#include "Interactable_TrapSwitch.h"

#include "InteractableData.h"
#include "DebugMeshes.h"

#include "Unit.h"

#include "SFXManager.h"
#include "PlayerController.h"

void Interactable_TrapSwitch::Start()
{
	auto ts = GetGameObject()->GetTransform();
	ts->SetWorldPosition(initPos);
	ts->SetWorldRotation(initRotation);
	ts->SetWorldScale(initScale);

	auto rendererObj = GetGameObject()->AddGameObject();
	AttachDebugMesh(rendererObj, DebugMeshType::Cube, yunuGI::Color::green());
	rendererObj->GetTransform()->SetLocalScale(Vector3d(2.5, 2.5, 2.5));
	auto boxCollider = GetGameObject()->AddComponent<physics::BoxCollider>();
	boxCollider->SetHalfExtent(Vector3d(1.25, 1.25, 1.25));

	for (auto each : GetGameObject()->GetChildren())
	{
		auto renderer = each->GetComponent<graphics::StaticMeshRenderer>();
		if (renderer)
		{
			mesh = each;
			break;
		}
	}

	for (auto each : targetInteractables)
	{
		each->OnExitCallBack.push_back([&]() { isInteracting = false; });
	}
}

void Interactable_TrapSwitch::Update()
{
	if (triggerOn)
	{
		if (mesh)
		{
			mesh->GetTransform()->SetLocalPosition(Vector3d(0, -offset_Y, 0));
		}
	}
	else
	{
		if (mesh)
		{
			mesh->GetTransform()->SetLocalPosition(Vector3d(0, 0, 0));
		}
	}

	/// 내부로 들어온 유닛이 죽었을 때, 리스트에서 제외하는 로직 필요함
}

void Interactable_TrapSwitch::OnTriggerEnter(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		PlayerController::Instance().GetState() == PlayerController::State::Battle &&
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->IsPlayerUnit() &&
		colliderUnitComponent->IsAlive())
	{
		triggerStay.insert(collider);
		triggerOn = true;
		if (triggerStay.size() == 1 && !isInteracting)
		{
			OnInteractableTriggerEnter();
			isInteracting = true;
			SFXManager::PlaySoundfile3D("sounds/trap/Scaffold trigger.wav", GetGameObject()->GetTransform()->GetWorldPosition());
		}
	}
}

void Interactable_TrapSwitch::OnTriggerExit(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->IsPlayerUnit())
	{
		if (triggerStay.size() == 1 && triggerOn)
		{
			triggerOn = false;
		}
		triggerStay.erase(collider);
	}
}

void Interactable_TrapSwitch::SetDataFromEditorData(const application::editor::InteractableData& data)
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
