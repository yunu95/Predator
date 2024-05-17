#pragma once
#include "YunutyEngine.h"
#include "YunuGraphicsInterface.h"

class TestComponent2 : virtual public yunutyEngine::Component
{
public:
	yunutyEngine::GameObject* obj;
	void SetActiveTrueObj()
	{
		obj->SetSelfActive(true);
	}

	void SetActiveFalseObj()
	{
		obj->SetSelfActive(false);
	}

	void Update()
	{
		//if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::U))
		//{
		//	effect->Appear();
		//}
		//if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::I))
		//{
		//	effect->DisAppear();
		//}
		//if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::O))
		//{
		//}
		//if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::P))
		//{
		//	anim->GetGI().ChangeAnimation(battleStartAnimation, 0.5, 1);
		//}
	}
};

