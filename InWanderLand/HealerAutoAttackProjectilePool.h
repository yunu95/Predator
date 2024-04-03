#pragma once
#include "HealerAutoAttackProjectile.h"
#include "GameObjectPool.h"
#include "PassiveCakePool.h"
#include "DebugMeshes.h"
#include "StaticMeshRenderer.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "LazySingletonClass.h"

class HealerAutoAttackProjectilePool :
	public GameObjectPool<HealerAutoAttackProjectile>, public GHContents::LazySingletonClass<HealerAutoAttackProjectilePool>
{
public:
	virtual void ObjectInitializer(HealerAutoAttackProjectile* projectile) override
	{
		auto bulletGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Fork");
		bulletGameObject->SetParent(projectile->GetGameObject());

		projectile->GetGameObject()->AddComponent<Dotween>();

		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectVector(projectile->GetGameObject());
	}
};

