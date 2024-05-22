#include "Interactable_SpikeTrap.h"

#include "InteractableData.h"
#include "DebugMeshes.h"

#include "Unit.h"

void Interactable_SpikeTrap::Start()
{
	auto ts = GetGameObject()->GetTransform();
	ts->SetWorldPosition(initPos);
	ts->SetWorldRotation(initRotation);
	ts->SetWorldScale(initScale);

	auto rendererObj = GetGameObject()->AddGameObject();
	AttachDebugMesh(rendererObj, DebugMeshType::Cube, yunuGI::Color::green());
	rendererObj->GetTransform()->SetLocalScale(Vector3d(3, 3, 3));
	auto boxCollider = GetGameObject()->AddComponent<physics::BoxCollider>();
	boxCollider->SetHalfExtent(Vector3d(1.5, 1.5, 1.5));

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

void Interactable_SpikeTrap::OnTriggerEnter(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
	{
		triggerStay.insert(collider);
		OnInteractableTriggerEnter();
	}
}

void Interactable_SpikeTrap::OnTriggerExit(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
	{
		if (triggerStay.size() == 1)
		{
			OnInteractableTriggerExit();
		}
		triggerStay.erase(collider);
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
