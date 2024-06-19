/// 2024. 05. 17 김상준
/// SpikeTrap 용 InteractableComponent

#pragma once

#include "IInteractableComponent.h"

class Unit;
class Interactable_SpikeTrap
	: public IInteractableComponent
{
public:
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnTriggerEnter(physics::Collider* collider) override;
	virtual void OnTriggerExit(physics::Collider* collider) override;

	virtual yunutyEngine::coroutine::Coroutine DoInteraction() override;

	virtual void SetDataFromEditorData(const application::editor::InteractableData& data) override;

private:
	std::unordered_set<Unit*> triggerStay = std::unordered_set<Unit*>();
	std::unordered_set<Unit*> interactingList = std::unordered_set<Unit*>();
	Vector3d initPos = Vector3d(0, 0, 0);
	Quaternion initRotation = Quaternion();
	Vector3d initScale = Vector3d(1, 1, 1);
	float delayTime = 0;
	float damage = 0;
	GameObject* spike = nullptr;
	GameObject* frame = nullptr;
};

