#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include "Vector3.h"
#include "unordered_set"
#include "Projectile.h"
#include "Bullet.h"
//#include "ProjectileManager.h"
#include <concepts>

// static void CreateProjectile() 이 있는 클래스만 템플릿 파라미터로 받을 것을 보장해주는 concept입니다.
template <class T>
concept ProjectileTypeConcept = requires(T t) {
	{ T::CreateProjectile() } -> std::same_as<void>;
};

/// <summary>
/// Projectile들을 보관해두는 ProjectilePool 클래스입니다.
/// 위에서 지정한 concept 문을 통해 static void CreateProjectile()이 있는 
/// 클래스만을 템플릿 파라미터로 받게 됩니다.
/// 이 클래스에는 Projectile을 Pool에서 꺼내고, 다시 회수하는 기능이 있습니다.
/// 그리고 Pool은 ProjectileType마다 하나씩 있기 때문에 Singleton으로 구현됩니다.
/// </summary>

template <ProjectileTypeConcept ProjectileType>
class ProjectilePool : public yunutyEngine::Component
{
public:
	ProjectilePool();
	static ProjectilePool<ProjectileType>* GetInstance() { return instance; }
	Projectile* SummonProjectile(yunutyEngine::Vector3d position, yunutyEngine::Vector3d speed, float damage, bool isPlayerProjectile);
	void RetrieveProjectile(Projectile* projectile);

private:
	static ProjectilePool<ProjectileType>* instance;
	std::unordered_set<Projectile*> projectiles;
	//friend class ProjectileManager;
};


template<ProjectileTypeConcept ProjectileType>
ProjectilePool<ProjectileType>* ProjectilePool<ProjectileType>::instance = nullptr;

template <ProjectileTypeConcept ProjectileType>
ProjectilePool<ProjectileType>::ProjectilePool()
{
	instance = this;
	//ProjectileManager::GetInstance()->projectilePoolList.insert(instance); 
	instance->projectiles.clear();
}

template <ProjectileTypeConcept ProjectileType>
Projectile* ProjectilePool<ProjectileType>::SummonProjectile(yunutyEngine::Vector3d position, yunutyEngine::Vector3d speed, float damage, bool isPlayerProjectile)
{
	if (instance == nullptr || instance->projectiles.empty())
	{	
		for (int i = 0; i < 10; ++i)
			ProjectileType::CreateProjectile();
	}
	auto it = instance->projectiles.begin();
	auto pProj = *it;
	pProj->GetTransform()->SetWorldPosition(position);
	pProj->speed = speed;
	pProj->damage = damage;
	pProj->isPlayerProjectile = isPlayerProjectile;
	pProj->GetGameObject()->SetSelfActive(true);
	instance->projectiles.erase(it);
	return pProj;
}

template <ProjectileTypeConcept ProjectileType>
void ProjectilePool<ProjectileType>::RetrieveProjectile(Projectile* projectile)
{
	instance->projectiles.insert(projectile);
	projectile->GetGameObject()->SetSelfActive(false);
}