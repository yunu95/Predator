#include "ChessPawn.h"

#include "InteractableData.h"
#include "DebugMeshes.h"
#include "Unit.h"
#include "TacticModeSystem.h"
#include "ChessBombComponent.h"
#include "YunutyWaitForSeconds.h"
#include "SFXManager.h"
#include "BossSummonChessSkill.h"
#include "GlobalConstant.h"
#include "ChessPool.h"
#include "ParticleTool_Manager.h"

namespace BossSummon
{
	void ChessPawn::Init(application::editor::ITemplateData* templateData)
	{
		/// chessBlockUnitLength
		/// chessBlockUnitOffset
		/// 두 값을 그냥 상수처럼 쓸 생각이라, 사실은
		/// Pool 에서 사용할 때, OnSummon 에서 갱신해줘야 하는 것들을 그냥 넘어감

		auto& gc = application::GlobalConstant::GetSingletonInstance().pod;

		auto rendererObj = GetGameObject()->AddGameObject();
		AttachDebugMesh(rendererObj, DebugMeshType::Cube, yunuGI::Color::green());
		rendererObj->GetTransform()->SetLocalScale(gc.chessBlockUnitLength * Vector3d::one);
		auto boxCollider = GetGameObject()->AddComponent<physics::BoxCollider>();
		boxCollider->SetHalfExtent(gc.chessBlockUnitLength * 0.5 * Vector3d::one);

		auto rookTemplate = static_cast<application::editor::Interactable_TemplateData*>(templateData);
		auto fbxObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(rookTemplate->pod.fBXName);
		fbxObj->SetParent(GetGameObject());
		for (auto each : fbxObj->GetChildren())
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
				bombObj->AddComponent<ChessBombComponent>();
				bombObj->GetTransform()->SetWorldScale(gc.chessBlockUnitLength * Vector3d::one);
				Vector3d combVector = (i - 1) * gc.chessBlockUnitLength * GetGameObject()->GetTransform()->GetWorldRotation().Forward().Normalized() + (j - 1) * gc.chessBlockUnitLength * GetGameObject()->GetTransform()->GetWorldRotation().Right().Normalized();
				bombObj->GetTransform()->SetLocalPosition(combVector + Vector3d((j - 1) * gc.chessBlockUnitOffset, guideUp_Y, (i - 1) * gc.chessBlockUnitOffset));
				bombObjList.push_back(bombObj);
				bombObj->SetSelfActive(false);
			}
		}

		particleObj = GetGameObject()->AddGameObject();
		auto pScale = GetGameObject()->GetTransform()->GetWorldScale();
		assert(pScale.x != 0 && pScale.y != 0 && pScale.z != 0);
		particleObj->GetTransform()->SetLocalScale(Vector3d(1 / pScale.x, 1 / pScale.y, 1 / pScale.z));
		auto pComp = particleObj->AddComponent<graphics::ParticleRenderer>();
		for (auto each : application::particle::ParticleTool_Manager::GetSingletonInstance().GetParticleList())
		{
			if (each.lock()->name == "DustParticle")
			{
				auto data = each.lock();
				pComp->SetParticleShape((yunutyEngine::graphics::ParticleShape)data->shape);
				pComp->SetParticleMode((yunutyEngine::graphics::ParticleMode)data->particleMode);
				pComp->SetLoop(data->isLoop);
				pComp->SetDuration(data->duration);
				pComp->SetLifeTime(data->lifeTime);
				pComp->SetSpeed(data->speed);
				pComp->SetStartScale(data->startScale);
				pComp->SetEndScale(data->endScale);
				pComp->SetMaxParticle(data->maxParticle);
				pComp->SetPlayAwake(data->playAwake);
				pComp->SetRadius(data->radius);
				pComp->SetAngle(data->angle);

				pComp->SetRateOverTime(data->rateOverTime);

				pComp->SetBurstsCount(data->burstsCount);
				pComp->SetInterval(data->interval);

				static const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

				std::wstring texturePath;
				texturePath.assign(data->texturePath.begin(), data->texturePath.end());
				auto texturePtr = resourceManager->GetTexture(texturePath);
				if (texturePtr)
				{
					pComp->SetTexture(texturePtr);
				}
				break;
			}
		}
		pComp->Play();
		particleObj->SetSelfActive(false);
	}

	void ChessPawn::OnSummon()
	{
		chessSummonedExplosionDelay = BossSummonChessSkill::pod.chessSummonedExplosionDelay;
		auto templateData = static_cast<application::editor::Interactable_TemplateData*>(ChessPool::Instance().GetPawnTemplate());
		damage = templateData->pod.damage;
		delayTime = templateData->pod.delayTime;
		particleEffectTime = templateData->pod.particleEffectTime;
		for (auto each : bombObjList)
		{
			each->GetComponent<ChessBombComponent>()->SetBombTime(delayTime);
		}
	}

	void ChessPawn::OnReturn()
	{
		ready = false;
		timerStart = false;
		triggerOn = false;
		isInteracting = false;
		mesh->GetTransform()->SetLocalPosition(Vector3d::zero);
		mesh->SetSelfActive(true);
		auto renderer = mesh->GetComponent<graphics::StaticMeshRenderer>();
		renderer->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::ALBEDO, orginTexture);
		particleObj->SetSelfActive(false);
		for (auto each : bombObjList)
		{
			each->SetSelfActive(false);
		}
		unitSet.clear();

		for (auto each : bombObjList)
		{
			each->GetComponent<ChessBombComponent>()->OnReturn();
		}
		localSummonedTime = 0;
		isPause = false;
	}

	void ChessPawn::OnBossDie()
	{
		if (GetGameObject()->GetSelfActive())
		{
			if (!lastCoroutine.expired())
			{
				DeleteCoroutine(lastCoroutine);
			}
			GetGameObject()->SetSelfActive(false);
		}
	}

	void ChessPawn::SetReady()
	{
		ready = true;
		particleObj->SetSelfActive(true);
		particleObj->GetComponent<graphics::ParticleRenderer>()->Play();
	}

	void ChessPawn::Update()
	{
		if (!ready || !timerStart || isPause)
		{
			return;
		}

		if (triggerOn)
		{
			if (!isInteracting)
			{
				lastCoroutine = StartCoroutine(DoInteraction());
				lastCoroutine.lock()->PushDestroyCallBack([this]()
					{
						if (GetGameObject()->GetSelfActive())
						{
							ChessPool::Instance().Return(std::static_pointer_cast<ChessPawn>(myWeakPtr.lock()));
						}
					});
				isInteracting = true;
			}
		}
		else
		{
			if (chessSummonedExplosionDelay == 0 || !unitSet.empty())
			{
				triggerOn = true;
			}
			else
			{
				localSummonedTime += yunutyEngine::Time::GetDeltaTime();
				float ratio = localSummonedTime / chessSummonedExplosionDelay;
				if (ratio >= 1)
				{
					triggerOn = true;
				}
			}
		}
	}

	void ChessPawn::OnTriggerEnter(physics::Collider* collider)
	{
		if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
			colliderUnitComponent != nullptr &&
			colliderUnitComponent->IsPlayerUnit() &&
			colliderUnitComponent->IsAlive())
		{
			unitSet.insert(colliderUnitComponent);
		}
	}

	void ChessPawn::OnTriggerExit(physics::Collider* collider)
	{
		if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
			colliderUnitComponent != nullptr &&
			colliderUnitComponent->IsPlayerUnit())
		{
			unitSet.erase(colliderUnitComponent);
		}
	}

	void ChessPawn::OnContentsStop()
	{
		if (!lastCoroutine.expired())
		{
			DeleteCoroutine(lastCoroutine);
		}
		GetComponent()->SetActive(false);
	}

	yunutyEngine::coroutine::Coroutine ChessPawn::DoInteraction()
	{
		auto& gc = application::GlobalConstant::GetSingletonInstance().pod;

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
					beforePos.x = -gc.vibeMaxOffset * ratio;
				}
				else
				{
					beforePos.x = gc.vibeMaxOffset * ratio;
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

	void ChessPawn::OnPause()
	{
		isPause = true;

		if (ready)
		{
			if (!timerStart)
			{
				if (particleObj)
				{
					particleObj->GetComponent<graphics::ParticleRenderer>()->Pause();
				}
			}
			else
			{
				for (auto each : bombObjList)
				{
					each->GetComponent<ChessBombComponent>()->OnPause();
				}
			}
		}
	}

	void ChessPawn::OnResume()
	{
		isPause = false;

		if (ready)
		{
			if (!timerStart)
			{
				if (particleObj)
				{
					particleObj->GetComponent<graphics::ParticleRenderer>()->Resume();
				}
			}
			else
			{
				for (auto each : bombObjList)
				{
					each->GetComponent<ChessBombComponent>()->OnResume();
				}
			}
		}
	}
}
