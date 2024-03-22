#pragma once
#include "AutoAttackProjectile.h"
#include "GameObjectPool.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"
#include "ContentsLayer.h"
#include "Application.h"

class AutoAttackProjectilePool : public GameObjectPool<AutoAttackProjectile>, public SingletonClass<AutoAttackProjectilePool>
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
	
		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectVector(projectile->GetGameObject());
	}
};