#pragma once
#include "YunutyEngine.h"

class TestComponent2 : public yunutyEngine::Component
{
public:
	graphics::Animator* anim;
	yunuGI::IAnimation* walkAnimation;
	yunuGI::IAnimation* idleAnimation;

	void Update()
	{
		if (Input::isKeyDown(KeyCode::H))
		{
			anim->GetGI().ChangeAnimation(walkAnimation, 1.f, 1.f);
		}
		if (Input::isKeyDown(KeyCode::J))
		{
			anim->GetGI().ChangeAnimation(idleAnimation, 1.f, 1.f);
		}
	}
};

