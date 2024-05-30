#include "ChessBombComponent.h"

#include "DebugMeshes.h"
#include "GlobalConstant.h"
#include "Unit.h"

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
	/// 폭탄 파티클 만들어서 붙이기
	particleObj->SetSelfActive(false);

	GetGameObject()->GetTransform()->SetLocalScale(GetGameObject()->GetTransform()->GetLocalScale());
}

void ChessBombComponent::Update()
{
	if (!coroutineStart)
	{
		StartCoroutine(AwakeBomb());
		coroutineStart = true;
	}
}

void ChessBombComponent::OnTriggerEnter(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
	{
		unitSet.insert(colliderUnitComponent);
	}
}

void ChessBombComponent::OnTriggerExit(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
	{
		unitSet.erase(colliderUnitComponent);
	}
}

yunutyEngine::coroutine::Coroutine ChessBombComponent::AwakeBomb()
{
	float localTimer = 0;
	assert(bombTime > 0 && "bombTime must be greater than 0");
	float ratio = localTimer / bombTime;

	while (ratio < 1)
	{
		localTimer += yunutyEngine::Time::GetDeltaTime();
		ratio = localTimer / bombTime;
		auto renderer = guideObj->GetComponent<graphics::StaticMeshRenderer>();
		renderer->GetGI().GetMaterial()->SetColor(yunuGI::Color(1, 1, 1, ratio));
		co_await std::suspend_always();
	}

	guideObj->SetSelfActive(false);
	particleObj->SetSelfActive(true);
	co_return;
}
