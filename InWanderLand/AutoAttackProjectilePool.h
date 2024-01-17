#pragma once
#include "AutoAttackProjectile.h"
#include "GameObjectPool.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"

class AutoAttackProjectilePool : public GameObjectPool<AutoAttackProjectile>, public SingletonClass<AutoAttackProjectilePool>
{	
public:
	virtual void ObjectInitializer(AutoAttackProjectile* projectile) override 
	{
		const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		auto capsuleMesh = _resourceManager->GetMesh(L"Capsule");

		auto projectileComponent = projectile->GetGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		projectileComponent->GetGI().SetMesh(capsuleMesh);
		projectileComponent->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 1, 0 });
	}
};