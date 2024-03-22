#pragma once
#include "MagicianAutoAttackProjectile.h"
#include "GameObjectPool.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"
#include "ContentsLayer.h"
#include "Application.h"

class MagicianAutoAttackProjectilePool :
	public GameObjectPool<MagicianAutoAttackProjectile>, public SingletonClass<MagicianAutoAttackProjectilePool>
{
public:
	virtual void ObjectInitializer(MagicianAutoAttackProjectile* projectile) override
	{
		//const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		//auto capsuleMesh = _resourceManager->GetMesh(L"Capsule");

		//auto projectileComponent = projectile->GetGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		//projectileComponent->GetGI().SetMesh(capsuleMesh);
		//projectileComponent->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));	
		auto bulletGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Fork");
		bulletGameObject->SetParent(projectile->GetGameObject());

		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectVector(projectile->GetGameObject());
		//bulletGameObject->GetTransform()->SetLocalRotation(Quaternion(Vector3d( 90, 0, 0 )));
		//bulletGameObject->GetTransform()->SetLocalRotation(projectile->GetGameObject()->GetTransform()->GetWorldRotation());
	}
};

