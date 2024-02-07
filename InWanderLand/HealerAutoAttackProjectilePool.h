#pragma once
#include "HealerAutoAttackProjectile.h"
#include "GameObjectPool.h"
#include "PassiveCakePool.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"

class HealerAutoAttackProjectilePool :
	public GameObjectPool<HealerAutoAttackProjectile>, public SingletonClass<HealerAutoAttackProjectilePool>
{
public:
	virtual void ObjectInitializer(HealerAutoAttackProjectile* projectile) override
	{
		projectile->GetGameObject()->SetSelfActive(false);
		const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		auto capsuleMesh = _resourceManager->GetMesh(L"Capsule");

		auto projectileComponent = projectile->GetGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		projectileComponent->GetGI().SetMesh(capsuleMesh);
		projectileComponent->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::blue(), false));
	}
};

