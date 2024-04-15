#pragma once
#include "YunutyEngine.h"
#include "YunuGraphicsInterface.h"

class TestComponent2 : public yunutyEngine::Component
{
public:
	BurnEffect* effect;

	void Update()
	{
		if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::U))
		{
			effect->Appear();
		}
		if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::I))
		{
			effect->DisAppear();
		}
		if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::O))
		{
		}
		//if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::P))
		//{
		//	anim->GetGI().ChangeAnimation(battleStartAnimation, 0.5, 1);
		//}
	}
};

