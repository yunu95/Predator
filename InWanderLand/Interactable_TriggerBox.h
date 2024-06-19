/// 2024. 05. 20 김상준
/// Interactable 의 trigger 용 InteractableComponent
/// Box

#pragma once

#include "IInteractableComponent.h"

class Unit;
class Interactable_TriggerBox
	: public IInteractableComponent
{
public:
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnTriggerEnter(physics::Collider* collider) override;
	virtual void OnTriggerExit(physics::Collider* collider) override;

	virtual void SetDataFromEditorData(const application::editor::InteractableData& data) override;

	virtual yunutyEngine::coroutine::Coroutine DoInteraction() override;

private:
	std::unordered_set<Unit*> triggerStay = std::unordered_set<Unit*>();
	Vector3d initPos = Vector3d(0, 0, 0);
	Quaternion initRotation = Quaternion();
	Vector3d initScale = Vector3d(1, 1, 1);
	bool activeInteractable = false;
	bool repetition = false;
	Vector2d uiOffset = Vector2d();
	std::string guideUI = "Texture/Interactable/UI_Interaction.png";
	float ui_Width = -1;
	float ui_Height = -1;
	GameObject* uiImage = nullptr;
};

