#pragma once

#include "IInteractableComponent.h"

class Interactable_BarricadeBox
	: public IInteractableComponent
{
public:
	virtual void Start() override;
	virtual void Update() {}

	virtual void OnTriggerEnter(physics::Collider* collider) override;

	virtual void SetDataFromEditorData(const application::editor::InteractableData& data) override;

	virtual yunutyEngine::coroutine::Coroutine DoInteraction() override { co_return; }

private:
	Vector3d initPos = Vector3d(0, 0, 0);
	Quaternion initRotation = Quaternion();
	Vector3d initScale = Vector3d(1, 1, 1);
};

