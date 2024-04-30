#pragma once
#include "AutoAttackProjectile.h"
#include "GameObjectPool.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"
#include "ContentsObservee.h"

class AutoAttackProjectilePool : public GameObjectPool<AutoAttackProjectile>, public Component, public SingletonComponent<AutoAttackProjectilePool>, public ContentsObservee
{	
public:
	virtual void ObjectInitializer(AutoAttackProjectile* projectile) override 
	{
		projectile->GetGameObject()->SetSelfActive(false);
		const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		auto capsuleMesh = _resourceManager->GetMesh(L"Capsule");

		auto projectileComponent = projectile->GetGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		projectileComponent->GetGI().SetMesh(capsuleMesh);
		projectileComponent->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::green(), false));	
	}
	virtual void Start() override;
	virtual void PlayFunction() override;
	virtual void StopFunction() override;
};

void AutoAttackProjectilePool::Start()
{
	isSingletonComponent = true;
}

void AutoAttackProjectilePool::PlayFunction()
{
	this->SetActive(true);
	if (isOncePaused)
	{
		Start();
	}
}

void AutoAttackProjectilePool::StopFunction()
{
	poolObjects.clear();
	expendableObjects.clear();
}
