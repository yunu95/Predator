#pragma once
#include "YunutyEngine.h"
using namespace yunutyEngine;

class TestComponent : public yunutyEngine::Component
{
protected:
	void Update()
	{
		if (Input::isKeyDown(KeyCode::LeftArrow))
		{
			Vector3d pos = GetTransform()->GetWorldPosition();
			pos.x -= 0.01f;
			GetTransform()->SetWorldPosition(pos);
		}

		if (Input::isKeyDown(KeyCode::RightArrow))
		{
			Vector3d pos = GetTransform()->GetWorldPosition();
			pos.x += 0.01f;
			GetTransform()->SetWorldPosition(pos);
		}

		if (Input::isKeyDown(KeyCode::UpArrow))
		{
			Vector3d pos = GetTransform()->GetWorldPosition();
			pos.y += 0.01f;
			GetTransform()->SetWorldPosition(pos);
		}
		if (Input::isKeyDown(KeyCode::DownArrow))
		{
			Vector3d pos = GetTransform()->GetWorldPosition();
			pos.y -= 0.01f;
			GetTransform()->SetWorldPosition(pos);
		}
	}
};
