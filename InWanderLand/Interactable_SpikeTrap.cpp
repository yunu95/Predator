#include "Interactable_SpikeTrap.h"

#include "InteractableData.h"
#include "DebugMeshes.h"

void Interactable_SpikeTrap::Start()
{
	auto ts = GetGameObject()->GetTransform();
	ts->SetWorldPosition(initPos);
	ts->SetWorldRotation(initRotation);
	ts->SetWorldScale(initScale);

	for (auto each : GetGameObject()->GetChildren())
	{
		auto renderer = each->GetComponent<graphics::StaticMeshRenderer>();
		if (renderer)
		{
			mesh = each;
			mesh->SetSelfActive(false);
			break;
		}
	}
}

void Interactable_SpikeTrap::Update()
{
	if (triggerOn)
	{
		if (!isInteracting)
		{
			lastCoroutine = StartCoroutine(DoInteraction());
		}
	}
	else
	{
		if (isInteracting)
		{
			mesh->SetSelfActive(false);
			DeleteCoroutine(lastCoroutine);
			isInteracting = false;
		}
	}
}

yunutyEngine::coroutine::Coroutine Interactable_SpikeTrap::DoInteraction()
{
	isInteracting = true;
	mesh->SetSelfActive(true);

	while(triggerOn)
	{
		co_await std::suspend_always();
	}

	co_return;
}

void Interactable_SpikeTrap::SetDataFromEditorData(const application::editor::InteractableData& data)
{
	initPos.x = data.pod.position.x;
	initPos.y = data.pod.position.y;
	initPos.z = data.pod.position.z;
	initRotation.w = data.pod.rotation.w;
	initRotation.x = data.pod.rotation.x;
	initRotation.y = data.pod.rotation.y;
	initRotation.z = data.pod.rotation.z;
	initScale.x = data.pod.scale.x;
	initScale.y = data.pod.scale.y;
	initScale.z = data.pod.scale.z;
}
