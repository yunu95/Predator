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
	this->currentTime += Time::GetDeltaTime();

	auto& gi = this->GetGI();

	auto currentAnimation = gi.GetCurrentAnimation();
	float duration = currentAnimation->GetDuration();
	int totalFrame = currentAnimation->GetTotalFrame();

	if (currentAnimation->GetLoop() && currentTime >= duration)
	{
		currentTime = 0.f;
	}

	float framePerSecond = totalFrame / duration;
	int currentFrame = this->currentTime / framePerSecond;

	gi.SetCurrentFrame(currentFrame);
	gi.SetFrameRatio((this->currentTime - currentFrame) / framePerSecond);
}
