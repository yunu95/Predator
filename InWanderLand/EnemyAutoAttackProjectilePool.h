#pragma once
#include "GameObjectPool.h"
#include "EnemyAutoAttackProjectile.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "LazySingletonClass.h"

class EnemyAutoAttackProjectilePool :
	public GameObjectPool<EnemyAutoAttackProjectile>, public GHContents::LazySingletonClass<EnemyAutoAttackProjectilePool>
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

		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectVector(projectile->GetGameObject());
	}
};

