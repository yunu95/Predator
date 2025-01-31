/// 2024. 05. 31 김상준
/// TrapSwitch 가 발동될 때 나오게 될 InteractableComponent

#pragma once

#include "IInteractableComponent.h"
#include "ITacticObject.h"

class Unit;
class Interactable_TrapArms
	: public IInteractableComponent, public ITacticObject
{
public:
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnTriggerEnter(physics::Collider* collider) override;
	virtual void OnTriggerExit(physics::Collider* collider) override;

	virtual yunutyEngine::coroutine::Coroutine DoInteraction() override;

	virtual void SetDataFromEditorData(const application::editor::InteractableData& data) override;

	virtual void OnPause() override;
	virtual void OnResume() override;
	
	virtual void Recovery() override;

private:
	std::unordered_set<Unit*> triggerStay = std::unordered_set<Unit*>();
	Vector3d initPos = Vector3d(0, 0, 0);
	Quaternion initRotation = Quaternion();
	Vector3d initScale = Vector3d(1, 1, 1);
	float delayTime = 0;
	float damage = 0;
	float offset_Y = 50.0f;
	float finalPos_Y = 24.3f;
	GameObject* mesh = nullptr;
	graphics::Animator* meshAnimator = nullptr;
	GameObject* particleObj = nullptr;
	bool isPause = false;
};

