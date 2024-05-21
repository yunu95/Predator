/// 2024. 05. 17 김상준
/// Chess_Pawn InteractableComponent

#pragma once

#include "IInteractableComponent.h"

class Interactable_ChessPawn
	: public IInteractableComponent
{
public:
	virtual void Start() override;
	virtual void Update() override;

	virtual yunutyEngine::coroutine::Coroutine DoInteraction() override;

	virtual void SetDataFromEditorData(const application::editor::InteractableData& data) override;

private:
	Vector3d initPos = Vector3d(0, 0, 0);
	Quaternion initRotation = Quaternion();
	Vector3d initScale = Vector3d(1, 1, 1);
};

