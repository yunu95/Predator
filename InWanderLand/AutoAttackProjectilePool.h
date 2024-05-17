#pragma once
#include "AutoAttackProjectile.h"
#include "GameObjectPool.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"
#include "PermanentObservee.h"

class AutoAttackProjectilePool : public GameObjectPool<AutoAttackProjectile>, public SingletonComponent<AutoAttackProjectilePool>, public PermanentObservee
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
	virtual void OnContentsStop() override;
};

void AutoAttackProjectilePool::Start()
{

}

void AutoAttackProjectilePool::OnContentsStop()
{
	this->SetActive(false);
	poolObjects.clear();
	expendableObjects.clear();
}
