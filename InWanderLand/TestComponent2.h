#pragma once
#include "YunutyEngine.h"
#include "YunuGraphicsInterface.h"

class TestComponent2 : public yunutyEngine::Component
{
public:
	graphics::StaticMeshRenderer* renderer;
	yunuGI::IShader* shader;
	yunuGI::IMaterial* material;

	void Update()
	{
		if (Input::isKeyPushed(KeyCode::I))
		{
		}
		if (Input::isKeyDown(KeyCode::K))
		{
		}
		if (Input::isKeyDown(KeyCode::J))
		{
			renderer->GetGI().GetMaterial()->SetPixelShader(shader);
		}
		if (Input::isKeyDown(KeyCode::L))
		{
			renderer->GetGI().SetMaterial(0, material);
		}
	}
};

