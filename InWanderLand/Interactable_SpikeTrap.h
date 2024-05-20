/// 2024. 05. 17 김상준
/// SpikeTrap 용 InteractableComponent

#pragma once

#include "IInteractableComponent.h"

class Interactable_SpikeTrap
	: public IInteractableComponent
{
public:
	virtual void Update() override;

	virtual yunutyEngine::coroutine::Coroutine DoInteraction() override;
};

