#pragma once
#include "GameObjectPool.h"
#include "EnemyAutoAttackProjectile.h"
#include "PermanentObservee.h"

class EnemyAutoAttackProjectilePool :
	public GameObjectPool<EnemyAutoAttackProjectile>, public SingletonComponent<EnemyAutoAttackProjectilePool>, public PermanentObservee
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
	virtual void OnContentsStop() override;
};

void EnemyAutoAttackProjectilePool::Start()
{

}

void EnemyAutoAttackProjectilePool::OnContentsStop()
{
	this->SetActive(false);
	poolObjects.clear();
	expendableObjects.clear();
}

