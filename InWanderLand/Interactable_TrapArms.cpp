#include "Interactable_TrapArms.h"

#include "InteractableData.h"
#include "DebugMeshes.h"

#include "Unit.h"
#include "GameManager.h"

#include "YunutyWaitForSeconds.h"
#include "ParticleTool_Manager.h"

void Interactable_TrapArms::Start()
{
	auto ts = GetGameObject()->GetTransform();
	ts->SetWorldPosition(initPos);
	ts->SetWorldRotation(initRotation);
	ts->SetWorldScale(initScale);

	auto rendererObj = GetGameObject()->AddGameObject();
	AttachDebugMesh(rendererObj, DebugMeshType::Cube, yunuGI::Color::green());
	rendererObj->GetTransform()->SetLocalScale(Vector3d(2, 2.5, 5));
	auto boxCollider = GetGameObject()->AddComponent<physics::BoxCollider>();
	boxCollider->SetHalfExtent(Vector3d(1, 1.25, 2.5));

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
	particleObj->SetSelfActive(false);
}

void Interactable_TrapArms::Update()
{
	if (triggerOn)
	{
		if (!isInteracting)
		{
			lastCoroutine = StartCoroutine(DoInteraction());
			isInteracting = true;
		}
		else if (lastCoroutine.expired() || lastCoroutine.lock()->Done())
		{
			isInteracting = false;
			OnInteractableTriggerExit();
		}
	}
}

void Interactable_TrapArms::OnTriggerEnter(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->IsPlayerUnit())
	{
		triggerStay.insert(collider);
	}
}

void Interactable_TrapArms::OnTriggerExit(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->IsPlayerUnit())
	{
		triggerStay.erase(collider);
	}
}

yunutyEngine::coroutine::Coroutine Interactable_TrapArms::DoInteraction()
{
	float animationTimer = 0;
	mesh->SetSelfActive(true);
	mesh->GetTransform()->SetLocalPosition(Vector3d(0, offset_Y, 0));

	float ratio = 0;
	if (delayTime != 0)
	{
		while (ratio < 1)
		{
			animationTimer += yunutyEngine::Time::GetDeltaTime();
			ratio = animationTimer / delayTime;
			if (ratio > 1)
			{
				ratio = 1;
			}
			mesh->GetTransform()->SetLocalPosition(Vector3d::Lerp(Vector3d(0, offset_Y, 0), Vector3d(0, (double)(offset_Y - 2.0f), 0), ratio));
			co_await std::suspend_always();
		}
	}

	ratio = 0;
	animationTimer = 0;
	while (ratio < 1)
	{
		animationTimer += yunutyEngine::Time::GetDeltaTime();
		ratio = animationTimer / 0.3;
		if (ratio > 1)
		{
			ratio = 1;
		}
		mesh->GetTransform()->SetLocalPosition(Vector3d::Lerp(Vector3d(0, (double)(offset_Y - 2.0f), 0), Vector3d::zero, ratio));
		co_await std::suspend_always();
	}
	mesh->GetTransform()->SetLocalPosition(Vector3d::zero);

	particleObj->SetSelfActive(true);
	particleObj->GetComponent<graphics::ParticleRenderer>()->Play();
	yunutyEngine::SoundSystem::PlaySoundfile3D("sounds/trap/Trigger_Impact.wav", GetGameObject()->GetTransform()->GetWorldPosition());

	for (auto each : triggerStay)
	{
		each->GetGameObject()->GetComponent<Unit>()->Damaged(damage);
	}

	co_yield yunutyEngine::coroutine::WaitForSeconds(0.5, false);

	ratio = 0;
	animationTimer = 0;
	while (ratio < 1)
	{
		animationTimer += yunutyEngine::Time::GetDeltaTime();
		ratio = animationTimer / 0.5;
		if (ratio > 1)
		{
			ratio = 1;
		}
		mesh->GetTransform()->SetLocalPosition(Vector3d::Lerp(Vector3d::zero, Vector3d(0, offset_Y, 0), ratio));
		co_await std::suspend_always();
	}

	mesh->SetSelfActive(false);
	particleObj->SetSelfActive(false);
	co_return;
}

void Interactable_TrapArms::SetDataFromEditorData(const application::editor::InteractableData& data)
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
	delayTime = data.pod.templateData->pod.delayTime;
	damage = data.pod.templateData->pod.damage;
}
