#pragma once
#include "HealerAutoAttackProjectile.h"
#include "GameObjectPool.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"
#include "PermanentObservee.h"

class HealerAutoAttackProjectilePool :
	public GameObjectPool<HealerAutoAttackProjectile>, public Component, public SingletonComponent<HealerAutoAttackProjectilePool>, public PermanentObservee
{
public:
	virtual void ObjectInitializer(HealerAutoAttackProjectile* projectile) override
	{
		auto bulletGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Fork");
		bulletGameObject->SetParent(projectile->GetGameObject());

		projectile->GetGameObject()->AddComponent<Dotween>();
	}
	virtual void Start() override;
	virtual void PlayFunction() override;
	virtual void StopFunction() override;
};

void HealerAutoAttackProjectilePool::Start()
{
}

void HealerAutoAttackProjectilePool::PlayFunction()
{
	this->SetActive(true);
	if (isOncePaused)
	{
		Start();
	}
}

void HealerAutoAttackProjectilePool::StopFunction()
{
	poolObjects.clear();
	expendableObjects.clear();
}

