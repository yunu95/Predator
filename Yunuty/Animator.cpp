#include "YunutyEngine.h"
#include "Animator.h"
#include "_YunuGIObjects.h"
using namespace yunutyEngine::graphics;

yunutyEngine::graphics::Animator::Animator()
	: Renderable<yunuGI::IAnimator>(_YunuGIObjects::SingleInstance().factory->CreateAnimator({}))
{

}

void Animator::Pause()
{
	isPlay = false;
}

void Animator::ClearAnimationEvent(yunuGI::IAnimation* animation)
{
	for (auto& [key, each] : this->animationEventMap[animation])
	{
		each.isFirst = true;
	}
}

void Animator::ChangeAnimation(yunuGI::IAnimation* animation, float transitionDuration, float transitionSpeed)
{
	auto& gi = this->GetGI();

	gi.ChangeAnimation(animation, transitionDuration, transitionSpeed);
}

void Animator::SetAnimationFrame(yunuGI::IAnimation* animation, unsigned int frame)
{
	isPlay = false;

	auto& gi = this->GetGI();
	auto& desc = gi.GetTransitionDesc();

	desc.curr.currFrame = frame;

	gi.Play(animation);
}

void Animator::Play(yunuGI::IAnimation* animation)
{
	this->isCurAnimationPlay = true;

	this->isPlay = true;

	auto& gi = this->GetGI();

	gi.Play(animation);
}

void Animator::PushAnimation(yunuGI::IAnimation* animation)
{
	auto& gi = this->GetGI();

	gi.PushAnimation(animation);
}

unsigned long long Animator::PushAnimationWithFunc(yunuGI::IAnimation* animation, unsigned int frame, std::function<void()> func)
{
	auto& gi = this->GetGI();

	gi.PushAnimation(animation);

	AnimationEvent animationEvent;
	animationEvent.frame = frame;
	animationEvent.func = func;
	animationEvent.isFirst = true;

	this->animationEventMap[animation][functorIndex] = animationEvent;
	return functorIndex++;

	//if (func)
	//{
	//	AnimationEvent animationEvent;
	//	animationEvent.frame = frame;
	//	animationEvent.func = func;
	//	animationEvent.isFirst = true;


	//	auto iter = this->animationEventMap.find(animation);

	//	if (iter == this->animationEventMap.end())
	//	{
	//		std::vector<AnimationEvent> tempVec;
	//		tempVec.emplace_back(animationEvent);
	//		this->animationEventMap.insert({ animation, tempVec });
	//	}
	//	else
	//	{
	//		for (auto& each : this->animationEventMap[animation])
	//		{
	//          /// frame 값이 같다고 해서 animationEvent 가 등록 안되게 하는지 이유를 잘 모르겠습니다.
	//          /// 특정 frame 에 여러 이벤트가 발생했을 때에도 대응해야 합니다.
	//			if (each.frame == frame)
	//			{
	//				return;
	//			}
	//		}

	//		this->animationEventMap[animation].emplace_back(animationEvent);
	//	}
	//}
}

bool Animator::EraseAnimationFunc(yunuGI::IAnimation* animation, unsigned long long index)
{
	if (animationEventMap.contains(animation) && animationEventMap[animation].contains(index))
	{
		animationEventMap[animation].erase(index);
		return true;
	}

	return false;
}

void Animator::Update()
{
	auto& gi = this->GetGI();
	auto& desc = gi.GetTransitionDesc();

	if (gi.GetCurrentAnimation() == nullptr)
	{
		return;
	}

	if (isPlay)
	{
		// 현재 애니메이션 업데이트
		{
			auto currentAnimation = gi.GetCurrentAnimation();
			float duration = currentAnimation->GetDuration();
			int totalFrame = currentAnimation->GetTotalFrame();
			desc.curr.speed = currentAnimation->GetPlaySpeed();
			__int32 ratio = static_cast<__int32>(totalFrame / duration);

			desc.curr.sumTime += (desc.curr.speed * Time::GetDeltaTime());
			if (desc.curr.sumTime >= currentAnimation->GetDuration())
			{
				this->isCurAnimationPlay = false;

				if (currentAnimation->GetLoop())
				{
					this->isCurAnimationPlay = true;
					desc.curr.sumTime = 0.f;
				}
			}
			desc.curr.currFrame = static_cast<__int32>(desc.curr.sumTime * ratio);
			desc.curr.currFrame = min(static_cast<int>(desc.curr.currFrame), totalFrame - 1);
			desc.curr.nextFrame = min(static_cast<int>(desc.curr.currFrame + 1), totalFrame - 1);
			desc.curr.ratio = static_cast<float>(desc.curr.sumTime - static_cast<float>(desc.curr.currFrame) / ratio);
		}

		for (auto& each : this->animationEventMap)
		{
			auto curAnimation = gi.GetCurrentAnimation();

			for (auto& [key, each2] : each.second)
			{
				if (curAnimation->GetName() == each.first->GetName())
				{
					if (each2.frame == desc.curr.currFrame)
					{
						if (each2.isFirst && (each2.func != nullptr))
						{
							each2.isFirst = false;
							each2.func();
						}
					}
					else
					{
						each2.isFirst = true;
					}
				}
			}
		}

		auto nextAnimation = gi.GetNextAnimation();
		// 다음 애니메이션이 있다면
		if (nextAnimation)
		{
			float duration = nextAnimation->GetDuration();
			int totalFrame = nextAnimation->GetTotalFrame();
			__int32 ratio = static_cast<__int32>(totalFrame / duration);

			desc.transitionTotalTime += (Time::GetDeltaTime() * desc.transitionSpeed);
			desc.transitionRatio = desc.transitionTotalTime / desc.transitionDuration;
			// 애니메이션 교체가 완료된다면
			if (desc.transitionRatio >= 1.f)
			{
				// 애니메이션 교체과 완료된다면 현재 애니메이션을 바꾸기 전에 현재 애니메이션에 등록된 이벤트를 클리어 해준다.
				ClearAnimationEvent(gi.GetCurrentAnimation());

				desc.curr = desc.next;
				desc.ClearNextAnimation();
				gi.SetCurrentAnimation(nextAnimation);
				gi.SetNextAnimation(nullptr);
			}
			else
			{
				desc.next.speed = nextAnimation->GetPlaySpeed();
				desc.next.sumTime += (desc.next.speed * Time::GetDeltaTime());
				if (desc.next.sumTime >= nextAnimation->GetDuration())
				{
					desc.next.sumTime = 0;
				}
				desc.next.currFrame = static_cast<__int32>(desc.next.sumTime * ratio);
				desc.next.currFrame = min(static_cast<int>(desc.next.currFrame), totalFrame - 1);
				desc.next.nextFrame = min(static_cast<int>(desc.next.currFrame + 1), totalFrame - 1);
				desc.next.ratio = static_cast<float>(desc.next.sumTime - static_cast<float>(desc.next.currFrame) / ratio);
			}
		}

		gi.SetTransitionDesc(desc);
	}
}
