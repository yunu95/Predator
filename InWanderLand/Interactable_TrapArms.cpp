#include "Interactable_TrapArms.h"

#include "InteractableData.h"
#include "DebugMeshes.h"

#include "Unit.h"

#include "YunutyWaitForSeconds.h"
#include "ParticleTool_Manager.h"
#include "SFXManager.h"

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

	meshAnimator = GetGameObject()->GetComponent<yunutyEngine::graphics::Animator>();

	for (auto each : GetGameObject()->GetChildren())
	{
		auto renderer = each->GetComponent<yunutyEngine::graphics::SkinnedMesh>();
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

			pComp->SetStartAngle(data->startAngle);
			pComp->SetEndAngle(data->endAngle);
			pComp->SetIsRandomScale(data->isRandomScale);
			pComp->SetIsRandomAngle(data->isRandomAngle);
			pComp->SetIsAlphaDiminish(data->isAlphaDiminish);

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

void Interactable_TrapArms::Update()
{
	static auto eraseList = triggerStay;
	for (auto each : triggerStay)
	{
		if (each->IsAlive())
		{
			eraseList.erase(each);
		}
	}
	for (auto each : eraseList)
	{
		triggerStay.erase(each);
	}

	eraseList.clear();

	if (!isPause && triggerOn)
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
	if (Unit* colliderUnitComponent = UnitCollider::AcquireUnit(collider);
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->IsAlive())
	{
		triggerStay.insert(colliderUnitComponent);
	}
}

void Interactable_TrapArms::OnTriggerExit(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = UnitCollider::AcquireUnit(collider);
		colliderUnitComponent != nullptr)
	{
		triggerStay.erase(colliderUnitComponent);
	}
}

yunutyEngine::coroutine::Coroutine Interactable_TrapArms::DoInteraction()
{
	float animationTimer = 0;
	mesh->SetSelfActive(true);

	yunuGI::IAnimation* anim_Up = nullptr;
	yunuGI::IAnimation* anim_Down = nullptr;

	for (auto each : graphics::Renderer::SingleInstance().GetResourceManager()->GetFBXAnimationList(L"SKM_Gimmick02"))
	{
		if (each->GetName() == L"Up")
		{
			anim_Up = each;
		}
		else if (each->GetName() == L"Down")
		{
			anim_Down = each;
		}
	}

	meshAnimator->Play(anim_Down);
	meshAnimator->Pause();
	mesh->GetTransform()->SetLocalPosition(Vector3d(0, finalPos_Y + offset_Y, 0));
	co_await std::suspend_always();

	float ratio = 0;
	if (delayTime != 0)
	{
		while (ratio < 1)
		{
			while (isPause)
			{
				co_await std::suspend_always();
			}

			animationTimer += yunutyEngine::Time::GetDeltaTime();
			ratio = animationTimer / delayTime;
			if (ratio > 1)
			{
				ratio = 1;
			}
			mesh->GetTransform()->SetLocalPosition(Vector3d::Lerp(Vector3d(0, finalPos_Y + offset_Y, 0), Vector3d(0, finalPos_Y, 0), ratio));
			co_await std::suspend_always();
		}
	}

	mesh->GetTransform()->SetLocalPosition(Vector3d(0, finalPos_Y, 0));
	meshAnimator->Resume();

	bool soundPlay = false;
	while (!meshAnimator->IsDone())
	{
		if (!soundPlay && meshAnimator->GetCurrentFrame() >= 19)
		{
			SFXManager::PlaySoundfile3D("sounds/trap/Scaffold Impact.wav", GetGameObject()->GetTransform()->GetWorldPosition());
			particleObj->SetSelfActive(true);
			particleObj->GetComponent<graphics::ParticleRenderer>()->Play();
			for (auto each : triggerStay)
			{
				each->Damaged(damage);
				each->KnockBackRelativeVector(0.5 * (each->GetTransform()->GetWorldPosition() - GetTransform()->GetWorldPosition()).Normalized(), 0.5);
			}
			soundPlay = true;
		}
		co_await std::suspend_always();
	}

	meshAnimator->Play(anim_Up);
	ratio = 0;
	animationTimer = 0;
	auto upLowTime = anim_Up->GetDuration() * 20 / anim_Up->GetTotalFrame();
	while (!meshAnimator->IsDone())
	{
		if (meshAnimator->GetCurrentFrame() >= 20)
		{
			animationTimer += yunutyEngine::Time::GetDeltaTime();
			ratio = animationTimer / upLowTime;
			if (ratio > 1)
			{
				ratio = 1;
			}
			mesh->GetTransform()->SetLocalPosition(Vector3d::Lerp(Vector3d(0, finalPos_Y, 0), Vector3d(0, finalPos_Y + offset_Y, 0), ratio));
		}
		else if (meshAnimator->GetCurrentFrame() >= 45)
		{
			break;
		}
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

void Interactable_TrapArms::OnPause()
{
	isPause = true;

	if (meshAnimator)
	{
		meshAnimator->Pause();
	}

	if (particleObj)
	{
		particleObj->GetComponent<graphics::ParticleRenderer>()->Pause();
	}
}

void Interactable_TrapArms::OnResume()
{
	isPause = false;

	if (meshAnimator)
	{
		meshAnimator->Resume();
	}

	if (particleObj)
	{
		particleObj->GetComponent<graphics::ParticleRenderer>()->Resume();
	}
}

void Interactable_TrapArms::Recovery()
{
	if (!lastCoroutine.expired())
	{
		DeleteCoroutine(lastCoroutine);
		isInteracting = false;
		if (mesh)
		{
			mesh->SetSelfActive(false);
		}
		if (particleObj)
		{
			particleObj->GetComponent<graphics::ParticleRenderer>()->Reset();
			particleObj->GetComponent<graphics::ParticleRenderer>()->Play();
			particleObj->SetSelfActive(false);
		}
	}
}
