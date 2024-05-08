#pragma once
#include "GameObjectPool.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"
#include "MagicianAutoAttackProjectile.h"
#include "PermanentObservee.h"


class MagicianSkillOneProjectilePool :
	public GameObjectPool<MagicianAutoAttackProjectile>, public Component, public SingletonComponent<MagicianSkillOneProjectilePool>, public PermanentObservee
{
public:
	virtual void ObjectInitializer(MagicianAutoAttackProjectile* projectile) override
	{
		const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		auto capsuleMesh = _resourceManager->GetMesh(L"Capsule");

		auto projectileComponent = projectile->GetGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		projectileComponent->GetGI().SetMesh(capsuleMesh);
		projectileComponent->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 0, 0 });
	}
	virtual void Start() override;
	virtual void PlayFunction() override;
	virtual void StopFunction() override;
};

void MagicianSkillOneProjectilePool::Start()
{
}

void MagicianSkillOneProjectilePool::PlayFunction()
{
	this->SetActive(true);
	if (isOncePaused)
	{
		Start();
	}
}

void MagicianSkillOneProjectilePool::StopFunction()
{
	poolObjects.clear();
	expendableObjects.clear();
}

