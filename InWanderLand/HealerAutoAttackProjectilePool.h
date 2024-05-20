#pragma once
#include "HealerAutoAttackProjectile.h"
#include "GameObjectPool.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"
#include "PermanentObservee.h"

class HealerAutoAttackProjectilePool :
	public GameObjectPool<HealerAutoAttackProjectile>, public SingletonComponent<HealerAutoAttackProjectilePool>, public Component, public PermanentObservee
{
public:
	virtual void ObjectInitializer(HealerAutoAttackProjectile* projectile) override
	{
		auto bulletGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Fork");
		bulletGameObject->SetParent(projectile->GetGameObject());
		projectile->Init();
		projectile->GetGameObject()->AddComponent<Dotween>();
	}
	virtual void Start() override;
	virtual void OnContentsStop() override;
	virtual Component* GetComponent() override { return this; }
};

void HealerAutoAttackProjectilePool::Start()
{

}

void HealerAutoAttackProjectilePool::OnContentsStop()
{
	this->SetActive(false);
	poolObjects.clear();
	expendableObjects.clear();
}

