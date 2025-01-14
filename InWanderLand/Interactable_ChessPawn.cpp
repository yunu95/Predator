#include "Interactable_ChessPawn.h"

#include "InteractableData.h"
#include "DebugMeshes.h"
#include "Unit.h"
#include "TacticModeSystem.h"
#include "ChessBombComponent.h"
#include "YunutyWaitForSeconds.h"
#include "SFXManager.h"
#include "BossSummonChessSkill.h"
#include "PlayerController.h"

void Interactable_ChessPawn::Start()
{
	auto ts = GetGameObject()->GetTransform();
	ts->SetWorldPosition(initPos);
	ts->SetWorldRotation(initRotation);
	ts->SetWorldScale(initScale);

	auto rendererObj = GetGameObject()->AddGameObject();
	AttachDebugMesh(rendererObj, DebugMeshType::Cube, yunuGI::Color::green());
	rendererObj->GetTransform()->SetLocalScale(chessBlockUnitLength * Vector3d::one);
	auto boxCollider = GetGameObject()->AddComponent<physics::BoxCollider>();
	boxCollider->SetHalfExtent(chessBlockUnitLength * 0.5 * Vector3d::one);

	for (auto each : GetGameObject()->GetChildren())
	{
		auto renderer = each->GetComponent<graphics::StaticMeshRenderer>();
		if (renderer)
		{
			mesh = each;
			const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
			orginTexture = resourceManager->GetTexture(L"FBX/SM_Chess_Pawn/SM_Chess_Pawn.fbm/T_Chess_Pawn_BaseColor.dds");
			flashTexture = resourceManager->GetTexture(L"Texture/Interactable/BombFlash.png");
			break;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			auto bombObj = GetGameObject()->AddGameObject();
			bombObj->AddComponent<ChessBombComponent>()->SetBombTime(delayTime);
			bombObj->GetTransform()->SetWorldScale(chessBlockUnitLength * Vector3d::one);
			Vector3d combVector = (i - 1) * chessBlockUnitLength * GetGameObject()->GetTransform()->GetWorldRotation().Forward().Normalized() + (j - 1) * chessBlockUnitLength * GetGameObject()->GetTransform()->GetWorldRotation().Right().Normalized();
			bombObj->GetTransform()->SetLocalPosition(combVector + Vector3d((j - 1) * chessBlockUnitOffset, guideUp_Y, (i - 1) * chessBlockUnitOffset));
			bombObjList.push_back(bombObj);
			bombObj->SetSelfActive(false);
		}
	}
}

void Interactable_ChessPawn::Update()
{
	static auto eraseList = unitSet;
	for (auto each : unitSet)
	{
		if (each->IsAlive())
		{
			eraseList.erase(each);
		}
	}
	for (auto each : eraseList)
	{
		unitSet.erase(each);
	}

	eraseList.clear();

	if (!unitSet.empty())
	{
		OnInteractableTriggerEnter();
	}

	if (!isPause && triggerOn)
	{
		if (!isInteracting)
		{
			lastCoroutine = StartCoroutine(DoInteraction());
			lastCoroutine.lock()->PushDestroyCallBack([this]()
				{
					if (!GetWeakPtr<Interactable_ChessPawn>().expired())
					{
						OnInteractableTriggerExit();
						GetGameObject()->SetSelfActive(false);
					}
				});
			isInteracting = true;
		}
	}
}

void Interactable_ChessPawn::OnTriggerEnter(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = UnitCollider::AcquireUnit(collider);
		colliderUnitComponent != nullptr && 
		colliderUnitComponent->IsPlayerUnit() &&
		colliderUnitComponent->IsAlive() && PlayerController::Instance().GetState() == PlayerController::State::Battle)
	{
		unitSet.insert(colliderUnitComponent);
	}
}

void Interactable_ChessPawn::OnTriggerExit(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = UnitCollider::AcquireUnit(collider);
		colliderUnitComponent != nullptr)
	{
		unitSet.erase(colliderUnitComponent);
	}
}

yunutyEngine::coroutine::Coroutine Interactable_ChessPawn::DoInteraction()
{
	float localTimer = 0;
	assert(delayTime > 0 && "delayTime must be greater than 0");
	float ratio = localTimer / delayTime;

	for (auto each : bombObjList)
	{
		each->SetSelfActive(true);
	}

	while (ratio < 1)
	{
		while (isPause)
		{
			co_await std::suspend_always();
		}

		ratio = localTimer / delayTime;

		if (ratio > 1)
		{
			ratio = 1;
		}

		if (!TacticModeSystem::Instance().IsOperation())
		{
			auto beforePos = mesh->GetTransform()->GetLocalPosition();
			if (beforePos.x >= 0)
			{
				beforePos.x = -vibeMaxOffset * ratio;
			}
			else
			{
				beforePos.x = vibeMaxOffset * ratio;
			}
			mesh->GetTransform()->SetLocalPosition(beforePos);
		}

		auto renderer = mesh->GetComponent<graphics::StaticMeshRenderer>();
		if ((int)(ratio * 10) % 2 != 0)
		{
			renderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::ALBEDO, flashTexture);
		}
		else
		{
			renderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::ALBEDO, orginTexture);
		}

		co_await std::suspend_always();
		localTimer += yunutyEngine::Time::GetDeltaTime();
	}

	mesh->SetSelfActive(false);

	std::unordered_set<Unit*> target = std::unordered_set<Unit*>();

	for (auto each : bombObjList)
	{
		auto comp = each->GetComponent<ChessBombComponent>();
		for (auto unit : comp->GetUnitsInTrigger())
		{
			target.insert(unit);
		}

		if (target.size() == 3)
		{
			break;
		}
	}

	for (auto each : target)
	{
		each->Damaged(damage);
	}

	SFXManager::PlaySoundfile3D("sounds/trap/Explosion.mp3", GetTransform()->GetWorldPosition());

	if (particleEffectTime == 0)
	{
		co_return;
	}

	co_yield yunutyEngine::coroutine::WaitForSeconds(particleEffectTime, false);
}

void Interactable_ChessPawn::SetDataFromEditorData(const application::editor::InteractableData& data)
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
	chessBlockUnitLength = application::GlobalConstant::GetSingletonInstance().pod.chessBlockUnitLength;
	chessBlockUnitOffset = application::GlobalConstant::GetSingletonInstance().pod.chessBlockUnitOffset;
	vibeMaxOffset = application::GlobalConstant::GetSingletonInstance().pod.vibeMaxOffset;
	damage = data.pod.templateData->pod.damage;
	delayTime = data.pod.templateData->pod.delayTime;
	particleEffectTime = data.pod.templateData->pod.particleEffectTime;
}

void Interactable_ChessPawn::OnPause()
{
	isPause = true;

	for (auto each : bombObjList)
	{
		each->GetComponent<ChessBombComponent>()->OnPause();
	}
}

void Interactable_ChessPawn::OnResume()
{
	isPause = false;

	for (auto each : bombObjList)
	{
		each->GetComponent<ChessBombComponent>()->OnResume();
	}
}

void Interactable_ChessPawn::CurrentProgressSave()
{
	savedInteract = isInteracting;
	for (auto each : bombObjList)
	{
		auto comp = each->GetComponent<ChessBombComponent>();
		comp->CurrentProgressSave();
	}
}

void Interactable_ChessPawn::Recovery()
{
	if (!savedInteract)
	{
		if (!lastCoroutine.expired())
		{
			lastCoroutine.lock()->PushDestroyCallBack([this]()
				{
					if (!GetWeakPtr<Interactable_ChessPawn>().expired())
					{
						GetGameObject()->SetSelfActive(true);
					}
				});
			DeleteCoroutine(lastCoroutine);
		}

		isInteracting = false;
		GetGameObject()->SetSelfActive(true);
		mesh->SetSelfActive(true);
		mesh->GetTransform()->SetLocalPosition(Vector3d::zero);
		auto renderer = mesh->GetComponent<graphics::StaticMeshRenderer>();
		renderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::ALBEDO, orginTexture);
		for (auto each : bombObjList)
		{
			auto comp = each->GetComponent<ChessBombComponent>();
			comp->Recovery();
			each->SetSelfActive(false);
		}
	}
}
