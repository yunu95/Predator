/// 2024. 05. 20 김상준
/// Interactable 의 trigger 용 InteractableComponent
/// Box

#pragma once

#include "IInteractableComponent.h"

#include <iostream>

namespace application
{
	namespace editor
	{
		class InteractableData;
	}
}

class Interactable_TriggerBox
	: public IInteractableComponent
{
public:
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnTriggerEnter(physics::Collider* collider) override
	{
		if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
			colliderUnitComponent != nullptr &&
			colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
		{
			OnInteractableTriggerEnter(); std::cout << "Box In\n";
		}
	}

	virtual void OnTriggerExit(physics::Collider* collider) override
	{
		if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
			colliderUnitComponent != nullptr &&
			colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
		{
			OnInteractableTriggerExit(); std::cout << "Box Out\n";
		}
	}

	void SetDataFromEditorData(const application::editor::InteractableData& data);

	virtual yunutyEngine::coroutine::Coroutine DoInteraction() override;

private:
	Vector3d initPos = Vector3d(0, 0, 0);
	Quaternion initRotation = Quaternion();
	Vector3d initScale = Vector3d(1, 1, 1);
};

