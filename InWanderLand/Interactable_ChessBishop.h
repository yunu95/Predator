/// 2024. 05. 17 김상준
/// Chess_Bishop InteractableComponent

#pragma once

#include "IInteractableComponent.h"

class Interactable_ChessBishop
	: public IInteractableComponent
{
public:
	virtual void Update() override;

	virtual yunutyEngine::coroutine::Coroutine DoInteraction() override;
};

