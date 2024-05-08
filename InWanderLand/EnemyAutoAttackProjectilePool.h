#pragma once
#include "GameObjectPool.h"
#include "EnemyAutoAttackProjectile.h"
#include "PermanentObservee.h"

class EnemyAutoAttackProjectilePool :
	public GameObjectPool<EnemyAutoAttackProjectile>, public Component, public SingletonComponent<EnemyAutoAttackProjectilePool>, public PermanentObservee
{
public:
	virtual void ObjectInitializer(EnemyAutoAttackProjectile* projectile) override
	{
		const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		auto capsuleMesh = _resourceManager->GetMesh(L"Capsule");

		auto projectileComponent = projectile->GetGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		projectileComponent->GetGI().SetMesh(capsuleMesh);
		projectileComponent->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 1, 0.25f, 0 });

		projectile->GetGameObject()->AddComponent<Dotween>();
	}
	virtual void Start() override;
	virtual void PlayFunction() override;
	virtual void StopFunction() override;
};

void EnemyAutoAttackProjectilePool::Start()
{
}

void EnemyAutoAttackProjectilePool::PlayFunction()
{
	this->SetActive(true);
	if (isOncePaused)
	{
		Start();
	}
}

void EnemyAutoAttackProjectilePool::StopFunction()
{
	poolObjects.clear();
	expendableObjects.clear();
}

