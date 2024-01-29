#pragma once
#include "YunutyEngine.h"
#include "YunuGraphicsInterface.h"

class TestComponent2 : public yunutyEngine::Component
{
public:
	graphics::SkinnedMesh* renderer;
	yunuGI::IShader* shader;
	yunuGI::IMaterial* material;
	GameObject* obj;
	void Update()
	{
		if (Input::isKeyPushed(KeyCode::I))
		{
			obj->SetSelfActive(false);
		}
		if (Input::isKeyDown(KeyCode::K))
		{
			obj->SetSelfActive(true);
		}
		if (Input::isKeyPushed(KeyCode::J))
		{
			Scene::getCurrentScene()->DestroyGameObject(obj);
		}
		if (Input::isKeyDown(KeyCode::L))
		{
			renderer->GetGI().SetMaterial(0, material);
		}
	}
};

