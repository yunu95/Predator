#pragma once
#include "YunutyEngine.h"

class TestComponent2 : public yunutyEngine::Component
{
public:
	GameObject* gameObject;

	void Update()
	{
		if (Input::isKeyDown(KeyCode::I))
		{
			gameObject->GetTransform()->position.y += gameObject->GetTransform()->position.y * Time::GetDeltaTime() * 10;
		}
		if (Input::isKeyDown(KeyCode::K))
		{
			gameObject->GetTransform()->position.y -= gameObject->GetTransform()->position.y * Time::GetDeltaTime() * 10;
		}
		if (Input::isKeyDown(KeyCode::J))
		{
			gameObject->GetTransform()->position.x -= gameObject->GetTransform()->position.x * Time::GetDeltaTime() * 10;
		}
		if (Input::isKeyDown(KeyCode::L))
		{
			gameObject->GetTransform()->position.x += gameObject->GetTransform()->position.x * Time::GetDeltaTime() * 10;
		}
	}
};

