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
		auto bulletGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Fork");
		projectile->Init();
		bulletGameObject->SetParent(projectile->GetGameObject());
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

