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

	this->currentTime += Time::GetDeltaTime()*gi.GetPlaySpeed();


	auto currentAnimation = gi.GetCurrentAnimation();
	float duration = currentAnimation->GetDuration();
	int totalFrame = currentAnimation->GetTotalFrame();

	__int32 ratio = static_cast<__int32>(totalFrame / duration);

	if (currentAnimation->GetLoop() && currentTime >= duration)
	{
		currentTime = 0.f;
	}

	currentFrame = static_cast<__int32>(currentTime * ratio);
	currentFrame = min(currentFrame, totalFrame - 1);

	gi.SetCurrentFrame(currentFrame);
	gi.SetFrameRatio(static_cast<float>(currentTime - static_cast<float>(currentFrame) / ratio));
}
