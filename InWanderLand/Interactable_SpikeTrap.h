/// 2024. 05. 17 김상준
/// SpikeTrap 용 InteractableComponent

#pragma once

#include "IInteractableComponent.h"

class Interactable_SpikeTrap
	: public IInteractableComponent
{
public:
	virtual void Start() override;
	virtual void Update() override {}

	virtual void OnTriggerEnter(physics::Collider* collider) override;
	virtual void OnTriggerExit(physics::Collider* collider) override;

	virtual yunutyEngine::coroutine::Coroutine DoInteraction() override { co_return; }

	virtual void SetDataFromEditorData(const application::editor::InteractableData& data) override;

private:
	std::unordered_set<physics::Collider*> triggerStay = std::unordered_set<physics::Collider*>();
	Vector3d initPos = Vector3d(0, 0, 0);
	Quaternion initRotation = Quaternion();
	Vector3d initScale = Vector3d(1, 1, 1);
	float damage = 0;
	GameObject* mesh = nullptr;
};

