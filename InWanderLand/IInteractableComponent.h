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

	/// triggerOn / isInteracting 등의 변수를 잘 활용하여
	/// 적절한 Update 문을 작성합니다.
	virtual void Update() = 0;

	/// Interaction 시에 수행하게 될 동작입니다.
	virtual yunutyEngine::coroutine::Coroutine DoInteraction() = 0;

	/// Interactable 에서 다른 Interactable 로의 전파를 하게 될 경우,
	/// 전파 대상을 등록하는 함수입니다.
	void InsertInteractableTarget(IInteractableComponent* target)
	{
		targetInteractables.insert(target);
	}

	bool IsTriggerOn() { return triggerOn; }
	bool IsInteracting() { return isInteracting; }

protected:

	/// triggerOn 을 true 로 변환합니다.
	/// targetInteractables 가 있는 경우, 해당 targetInteractables 의 OnInteractableTriggerEnter 를 호출합니다.
	void OnInteractableTriggerEnter()
	{
		triggerOn = true;
		for (auto each : targetInteractables)
		{
			if (each->GetGameObject()->GetActive())
			{
				each->OnInteractableTriggerEnter();
			}
		}
	}

	/// triggerOn 을 false 로 변환합니다.
	/// targetInteractables 가 있는 경우, 해당 targetInteractables 의 OnInteractableTriggerExit 를 호출합니다.
	void OnInteractableTriggerExit()
	{
		triggerOn = false;
		for (auto each : targetInteractables)
		{
			if (each->GetGameObject()->GetActive())
			{
				each->OnInteractableTriggerExit();
			}
		}
	}

	std::unordered_set<IInteractableComponent*> targetInteractables = std::unordered_set<IInteractableComponent*>();

	bool triggerOn = false;
	bool isInteracting = false;
	std::weak_ptr<yunutyEngine::coroutine::Coroutine> lastCoroutine = std::weak_ptr<yunutyEngine::coroutine::Coroutine>();
};
