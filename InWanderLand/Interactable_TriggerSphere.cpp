#include "Interactable_TriggerSphere.h"

#include "InteractableData.h"
#include "DebugMeshes.h"
#include "UIManager.h"

#include "Unit.h"

void Interactable_TriggerSphere::Start()
{
	AttachDebugMesh(GetGameObject(), DebugMeshType::Sphere, yunuGI::Color::green());
	auto col = GetGameObject()->AddComponent<physics::SphereCollider>();
	col->SetRadius(0.5);

	auto ts = GetGameObject()->GetTransform();
	ts->SetWorldPosition(initPos);
	ts->SetWorldRotation(initRotation);
	ts->SetWorldScale(initScale);

	if (activeInteractable)
	{
		uiImage = GetGameObject()->AddGameObject();
		auto uiComp = uiImage->AddComponent<Interactable_UI>();
		uiComp->SetUI(guideUI);
		uiComp->SetUIWidth(ui_Width);
		uiComp->SetUIHeight(ui_Height);
	}
}

void Interactable_TriggerSphere::Update()
{
	/// 개선 필요
	if (uiImage)
	{
		auto scale = GetGameObject()->GetTransform()->GetWorldScale();
		uiImage->GetTransform()->SetLocalScale(Vector3d(1 / scale.x, 1 / scale.y, 1 / scale.z));
		auto pos = UIManager::Instance().GetUIPosFromWorld(GetGameObject()->GetTransform()->GetWorldPosition());
		uiImage->GetTransform()->SetWorldPosition(Vector3d((pos.x + uiOffset.x) / scale.x, (pos.y + uiOffset.y) / scale.y, 0));
	}

	if (activeInteractable && yunutyEngine::Input::isKeyPushed(KeyCode::Space) && uiImage->GetSelfActive())
	{
		if (repetition)
		{
			OnInteractableTriggerEnter();
		}
		else
		{
			if (!isInteracting)
			{
				OnInteractableTriggerEnter();
				uiImage->SetSelfActive(false);
				isInteracting = true;
			}
		}
	}

	/// 내부로 들어온 유닛이 죽었을 때, 리스트에서 제외하는 로직 필요함
}

void Interactable_TriggerSphere::OnTriggerEnter(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->IsPlayerUnit() &&
		colliderUnitComponent->IsAlive())
	{
		triggerStay.insert(collider);
		if (activeInteractable)
		{
			if (repetition)
			{
				uiImage->SetSelfActive(true);
			}
			else
			{
				if (!isInteracting)
				{
					uiImage->SetSelfActive(true);
				}
			}
		}
		else
		{
			OnInteractableTriggerEnter();
		}
	}
}

void Interactable_TriggerSphere::OnTriggerExit(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->IsPlayerUnit())
	{
		if (triggerStay.size() == 1)
		{
			if (activeInteractable)
			{
				uiImage->SetSelfActive(false);
				if (triggerOn)
				{
					OnInteractableTriggerExit();
					if (!repetition)
					{
						GetGameObject()->SetSelfActive(false);
					}
				}
			}
			else
			{
				OnInteractableTriggerExit();
			}
		}
		triggerStay.erase(collider);
	}
}

void Interactable_TriggerSphere::SetDataFromEditorData(const application::editor::InteractableData& data)
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
	activeInteractable = data.pod.templateData->pod.activeInteractable;
	repetition = data.pod.templateData->pod.repetition;
	uiOffset.x = data.pod.uiOffset.x;
	uiOffset.y = data.pod.uiOffset.y;
	guideUI = data.pod.guideUI;
	ui_Width = data.pod.ui_Width;
	ui_Height = data.pod.ui_Height;
}

yunutyEngine::coroutine::Coroutine Interactable_TriggerSphere::DoInteraction()
{
	/// 해당 Coroutine 은 호출되지 않을 것입니다.
	co_return;
}
