/// 2024. 05. 17
/// Interactable 요소를 만들기 위한 Component 의 추상 클래스

#pragma once

#include "YunutyEngine.h"
#include "ContentsObservee.h"

#include <unordered_set>

class IInteractableComponent
	: public Component, public ContentsObservee
{
public:
	virtual ~IInteractableComponent() = default;
	virtual Component* GetComponent() override { return this; }

	virtual void Update() override
	{
		//if (IsTriggerOn())
		//{
		//	DoInteraction();
		//}
	}

	//virtual bool IsTriggerOn() = 0;

	/// IsTriggerOn 이 true 일 경우나, 
	/// 반드시 isInteracting 에 대한 조작을 내부에서 수행해야 합니다.
	//virtual void DoInteraction() = 0;

	/// Interactable 에서 다른 Interactable 로의 전파를 하게 될 경우,
	/// 전파 대상을 등록하는 함수입니다.
	/// 해당 container 를 통해서 다른 Interactable 의 DoInteraction 을 수행하도록 설계할 수 있습니다.
	void InsertInteractableTarget(IInteractableComponent* target)
	{
		targetInteractables.insert(target);
	}

	bool IsInteracting() { return isInteracting; }

protected:
	std::unordered_set<IInteractableComponent*> targetInteractables = std::unordered_set<IInteractableComponent*>();

	/// DoInteraction 을 수행할 경우에, 반드시 해당 flag 를 true 로 변경하고,
	/// 특정한 동작을 마칠 경우에 다시 false 로 처리할 수 있도록 합니다.
	/// DoInteraction 에서 IsInteractive 를 통해
	bool isInteracting = false;
};
