/// 2024. 05. 17
/// Interactable 요소를 만들기 위한 Component 의 추상 클래스

#pragma once

#include "YunutyEngine.h"
#include "ContentsObservee.h"

#include "Interactable_UI.h"

#include <unordered_set>
#include <vector>
#include <functional>

namespace application
{
	namespace editor
	{
		class InteractableData;
	}
}

class IInteractableComponent
	: public Component, public ContentsObservee
{
public:
	virtual ~IInteractableComponent() = default;

	/// triggerOn / isInteracting 등의 변수를 잘 활용하여
	/// 적절한 Update 문을 작성합니다.
	virtual void Update() = 0;

	/// Interaction 시에 수행하게 될 동작입니다.
	virtual yunutyEngine::coroutine::Coroutine DoInteraction() = 0;

	/// InteractableData 로부터 Component 의 데이터를 초기화하는 함수입니다.
	virtual void SetDataFromEditorData(const application::editor::InteractableData& data) {};

	/// Interactable 에서 다른 Interactable 로의 전파를 하게 될 경우,
	/// 전파 대상을 등록하는 함수입니다.
	void InsertInteractableTarget(IInteractableComponent* target)
	{
		targetInteractables.insert(target);
	}

	bool IsTriggerOn() { return triggerOn; }
	bool IsInteracting() { return isInteracting; }

	/// OnInteractableTriggerEnter 시에 호출할 CallBack 함수입니다.
	std::vector<std::function<void()>> OnEnterCallBack = std::vector<std::function<void()>>();

	/// OnInteractableTriggerExit 시에 호출할 CallBack 함수입니다.
	std::vector<std::function<void()>> OnExitCallBack = std::vector<std::function<void()>>();

	virtual Component* GetComponent() { return this; }

	virtual void CurrentProgressSave() {}
	virtual void Recovery() {}

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

		for (auto& each : OnEnterCallBack)
		{
			each();
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

		for (auto& each : OnExitCallBack)
		{
			each();
		}
	}

	std::unordered_set<IInteractableComponent*> targetInteractables = std::unordered_set<IInteractableComponent*>();

	bool triggerOn = false;
	bool isInteracting = false;
	std::weak_ptr<yunutyEngine::coroutine::Coroutine> lastCoroutine = std::weak_ptr<yunutyEngine::coroutine::Coroutine>();
};
