#pragma once
#include "MagicianAutoAttackProjectile.h"
#include "GameObjectPool.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"
#include "Dotween.h"
#include "PermanentObservee.h"

class MagicianAutoAttackProjectilePool :
	public GameObjectPool<MagicianAutoAttackProjectile>, public SingletonComponent<MagicianAutoAttackProjectilePool>, public Component, public PermanentObservee
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
	virtual void OnContentsStop() override;
	virtual Component* GetComponent() override { return this; }
};

void MagicianAutoAttackProjectilePool::Start()
{
}

void MagicianAutoAttackProjectilePool::OnContentsStop()
{
	this->SetActive(false);
	poolObjects.clear();
	expendableObjects.clear();
}

