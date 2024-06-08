#include "ChessBombComponent.h"

#include "DebugMeshes.h"
#include "GlobalConstant.h"
#include "Unit.h"
#include "ParticleTool_Manager.h"

void ChessBombComponent::Start()
{
	AttachDebugMesh(GetGameObject(), DebugMeshType::Cube, yunuGI::Color::green());
	GetGameObject()->AddComponent<physics::BoxCollider>();

	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	yunuGI::IMaterial* mat = resourceManager->GetMaterial(L"ChessDamageZone");
	if (mat == nullptr)
	{
		mat = resourceManager->CreateMaterial(L"ChessDamageZone");
		mat->SetTexture(yunuGI::Texture_Type::Temp0, resourceManager->GetTexture(L"Texture/Interactable/ChessDamageZone.png"));
		mat->SetPixelShader(resourceManager->GetShader(L"TexturePS.cso"));
	}

	guideObj = GetGameObject()->AddGameObject();
	auto guideRenderer = guideObj->AddComponent<graphics::StaticMeshRenderer>();
	guideRenderer->GetGI().SetMesh(resourceManager->GetMesh(L"Rectangle"));
	guideRenderer->GetGI().SetMaterial(0, mat);
	guideRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color(1, 1, 1, 0));
	guideObj->GetTransform()->SetLocalRotation(Vector3d(90, 0, 0));

	particleObj = GetGameObject()->AddGameObject();
	auto pScale = GetGameObject()->GetTransform()->GetWorldScale();
	assert(pScale.x != 0 && pScale.y != 0 && pScale.z != 0);
	particleObj->GetTransform()->SetLocalScale(Vector3d(1 / pScale.x, 1 / pScale.y, 1 / pScale.z));
	auto pComp = particleObj->AddComponent<graphics::ParticleRenderer>();
	for (auto each : application::particle::ParticleTool_Manager::GetSingletonInstance().GetParticleList())
	{
		if (each.lock()->name == "ChessBombParticle")
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

void ChessBombComponent::Update()
{
	if (!coroutineStart)
	{
		lastCoroutine = StartCoroutine(AwakeBomb());
		coroutineStart = true;
	}
}

void ChessBombComponent::OnTriggerEnter(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->IsPlayerUnit())
	{
		unitSet.insert(colliderUnitComponent);
	}
}

void ChessBombComponent::OnTriggerExit(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->IsPlayerUnit())
	{
		unitSet.erase(colliderUnitComponent);
	}
}

yunutyEngine::coroutine::Coroutine ChessBombComponent::AwakeBomb()
{
	float localTimer = 0;
	assert(bombTime > 0 && "bombTime must be greater than 0");
	float ratio = localTimer / bombTime;

	guideObj->SetSelfActive(true);

	while (ratio < 1)
	{
		ratio = localTimer / bombTime;
		auto renderer = guideObj->GetComponent<graphics::StaticMeshRenderer>();
		if (ratio > 1)
		{
			ratio = 1;
		}
		renderer->GetGI().GetMaterial()->SetColor(yunuGI::Color(1, 1, 1, ratio));
		co_await std::suspend_always();
		localTimer += yunutyEngine::Time::GetDeltaTime();
	}

	guideObj->SetSelfActive(false);
	particleObj->SetSelfActive(true);
	particleObj->GetComponent<graphics::ParticleRenderer>()->Play();
	co_return;
}

void ChessBombComponent::OnReturn()
{
	coroutineStart = false;
	unitSet.clear();

	if (particleObj)
	{
		particleObj->SetSelfActive(false);
	}

	if (!lastCoroutine.expired())
	{
		DeleteCoroutine(lastCoroutine);
	}
}
