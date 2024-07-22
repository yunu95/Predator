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

void yunutyEngine::graphics::Animator::Resume()
{
	isPlay = true;
}

void Animator::ClearAnimationEvent(yunuGI::IAnimation* animation)
{
	for (auto& [key, each] : this->animationEventMap[animation])
	{
		if (each.isFirst && each.must)
		{
			each.func();
		}
		each.isFirst = true;
	}
}

void Animator::ChangeAnimation(yunuGI::IAnimation* animation, float transitionDuration, float transitionSpeed)
{
	auto& gi = this->GetGI();

	gi.ChangeAnimation(animation, transitionDuration, transitionSpeed);
	isChange = true;
}
yunuGI::IAnimation* yunutyEngine::graphics::Animator::GetCurrentAnimation()
{
	return GetGI().GetCurrentAnimation();
}
bool Animator::IsDone()
{
	if (auto currentAnim = GetGI().GetCurrentAnimation(); currentAnim)
	{
		// 애니메이션 보간중이 아닐 때
		if (GetGI().GetTransitionDesc().next.animIndex == -1)
		{
			return GetGI().GetTransitionDesc().curr.sumTime >= currentAnim->GetDuration();
		}
		else
		{
			return false;
		}
	}
	return true;
}

void Animator::SetAnimationFrame(yunuGI::IAnimation* animation, unsigned int frame)
{
	isPlay = false;

	auto& gi = this->GetGI();

	gi.SetAnimationFrame(animation, frame);
}

void Animator::Play(yunuGI::IAnimation* animation)
{
	this->isPlay = true;

	auto& gi = this->GetGI();

	gi.Play(animation);

	auto nextAnimation = gi.GetNextAnimation();
	if (nextAnimation)
	{
		ClearAnimationEvent(nextAnimation);
		gi.GetTransitionDesc().ClearNextAnimation();
		gi.SetNextAnimation(nullptr);
	}
}

void Animator::PushAnimation(yunuGI::IAnimation* animation)
{
	auto& gi = this->GetGI();

	gi.PushAnimation(animation);
}

unsigned long long Animator::PushAnimationWithFunc(yunuGI::IAnimation* animation, unsigned int frame, std::function<void()> func, bool must)
{
	auto& gi = this->GetGI();

	gi.PushAnimation(animation);

	AnimationEvent animationEvent;
	animationEvent.frame = frame;
	animationEvent.func = func;
	animationEvent.isFirst = true;
	animationEvent.must = must;

	this->animationEventMap[animation][functorIndex] = animationEvent;
	return functorIndex++;
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

float yunutyEngine::graphics::Animator::GetCurrentFrame()
{
	auto& gi = this->GetGI();
	auto& desc = gi.GetTransitionDesc();

	return desc.curr.currFrame;
}

void Animator::OnEnable()
{
	if (this->GetGI().GetCurrentAnimation())
	{
		this->Play(this->GetGI().GetCurrentAnimation());
	}
}

void Animator::OnDisable()
{
	///ClearAnimationEvent(this->GetGI().GetCurrentAnimation());
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
			float frameDuration = duration / totalFrame; // 각 프레임의 시간 길이
			desc.curr.sumTime += (gi.GetPlaySpeed() * desc.curr.speed * Time::GetDeltaTime());

			if (desc.curr.sumTime >= currentAnimation->GetDuration())
			{
				if (currentAnimation->GetLoop())
				{
					desc.curr.sumTime -= currentAnimation->GetDuration();
				}
			}

			desc.curr.currFrame = static_cast<__int32>(desc.curr.sumTime / frameDuration);
			desc.curr.currFrame = min(static_cast<int>(desc.curr.currFrame), totalFrame - 1);
			desc.curr.nextFrame = min(static_cast<int>(desc.curr.currFrame + 1), totalFrame - 1);
			desc.curr.ratio = (desc.curr.sumTime - (desc.curr.currFrame * frameDuration)) / frameDuration;
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
			totalFrame = totalFrame + 1;
			float frameDuration = duration / totalFrame; // 각 프레임의 시간 길이
			desc.transitionTotalTime += (Time::GetDeltaTime() * desc.transitionSpeed);
			desc.transitionRatio = desc.transitionTotalTime / desc.transitionDuration;

			// 애니메이션 블렌딩이 끝난다면
			if (desc.transitionRatio >= 1.f)
			{
				// 애니메이션 블렌딩이 끝난다면 블렌딩에 쓰인 애니메이션을 클리어해준다.
				ClearAnimationEvent(gi.GetNextAnimation());

				desc.ClearNextAnimation();
				gi.SetNextAnimation(nullptr);
			}
			else
			{
				desc.next.speed = nextAnimation->GetPlaySpeed();
				desc.next.sumTime += (gi.GetPlaySpeed() * desc.next.speed * Time::GetDeltaTime());
				if (desc.next.sumTime >= nextAnimation->GetDuration())
				{
					if (nextAnimation->GetLoop())
					{
						desc.next.sumTime = 0;
					}
				}

				desc.next.currFrame = static_cast<__int32>(desc.next.sumTime / frameDuration);
				desc.next.currFrame = min(static_cast<int>(desc.next.currFrame), totalFrame - 1);
				desc.next.nextFrame = min(static_cast<int>(desc.next.currFrame + 1), totalFrame - 1);
				desc.next.ratio = (desc.next.sumTime - (desc.next.currFrame * frameDuration)) / frameDuration;

				// 블렌딩 중인 애니메이션에 대한 이벤트까지 처리
				for (auto& each : this->animationEventMap)
				{
					auto nextAnimation = gi.GetNextAnimation();
					for (auto& [key, each2] : each.second)
					{
						if (nextAnimation->GetName() == each.first->GetName())
						{
							if (each2.frame == desc.next.currFrame)
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
			}
		}

		gi.SetTransitionDesc(desc);
	}
}
