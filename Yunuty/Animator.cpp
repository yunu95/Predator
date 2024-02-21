#include "YunutyEngine.h"
#include "Animator.h"
#include "_YunuGIObjects.h"
using namespace yunutyEngine::graphics;

yunutyEngine::graphics::Animator::Animator()
	: Renderable<yunuGI::IAnimator>(_YunuGIObjects::SingleInstance().factory->CreateAnimator({}))
{

}

void Animator::Update()
{
	auto& gi = this->GetGI();
	auto& desc = gi.GetTransitionDesc();
	
	if (gi.GetCurrentAnimation() == nullptr)
	{
		return;
	}

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
			if (currentAnimation->GetLoop())
			{
				desc.curr.sumTime = 0.f;
			}
		}
		desc.curr.currFrame = static_cast<__int32>(desc.curr.sumTime * ratio);
		desc.curr.currFrame = min(static_cast<int>(desc.curr.currFrame), totalFrame - 1);
		desc.curr.nextFrame = min(static_cast<int>(desc.curr.currFrame+1), totalFrame -1);
		desc.curr.ratio = static_cast<float>(desc.curr.sumTime - static_cast<float>(desc.curr.sumTime) / ratio);
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
			desc.next.ratio = static_cast<float>(desc.next.sumTime - static_cast<float>(desc.next.sumTime) / ratio);
		}
	}

	gi.SetTransitionDesc(desc);
}
