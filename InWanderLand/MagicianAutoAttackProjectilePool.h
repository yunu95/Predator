#pragma once
#include "MagicianAutoAttackProjectile.h"
#include "GameObjectPool.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"
#include "Dotween.h"
#include "PermanentObservee.h"

class MagicianAutoAttackProjectilePool :
	public GameObjectPool<MagicianAutoAttackProjectile>, public Component, public SingletonComponent<MagicianAutoAttackProjectilePool>, public PermanentObservee
{
public:
	virtual void ObjectInitializer(MagicianAutoAttackProjectile* projectile) override
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

void MagicianAutoAttackProjectilePool::Start()
{
}

void MagicianAutoAttackProjectilePool::PlayFunction()
{
	this->SetActive(true);
	if (isOncePaused)
	{
		Start();
	}
}

void MagicianAutoAttackProjectilePool::StopFunction()
{
	poolObjects.clear();
	expendableObjects.clear();
}

