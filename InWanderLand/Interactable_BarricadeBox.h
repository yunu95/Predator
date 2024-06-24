#pragma once

#include "IInteractableComponent.h"

class Interactable_BarricadeBox
	: public IInteractableComponent
{
	virtual void Update() {}
	virtual yunutyEngine::coroutine::Coroutine DoInteraction() override { co_return; }
};

