#pragma once
#include "YunutyEngine.h"
#include "YunuGraphicsInterface.h"

class TestComponent2 : public yunutyEngine::Component
{
public:
	yunutyEngine::graphics::Animator* anim;
	yunuGI::IAnimation* idleAnimation;
	yunuGI::IAnimation* battleIdleAnimation;
	yunuGI::IAnimation* walkAnimation;
	yunuGI::IAnimation* battleStartAnimation;

	void Update()
	{
		if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::U))
		{
			anim->GetGI().ChangeAnimation(idleAnimation, 0.5, 1);
		}
		if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::I))
		{
			anim->GetGI().ChangeAnimation(battleIdleAnimation, 1, 1);
		}
		if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::O))
		{
			anim->GetGI().ChangeAnimation(walkAnimation, 1, 1);
		}
		if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::P))
		{
			anim->GetGI().ChangeAnimation(battleStartAnimation, 0.5, 1);
		}
	}
};

