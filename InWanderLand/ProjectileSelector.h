//#pragma once
//#include "YunutyEngine.h"
//#include "ProjectileType.h"
//
//struct Projectile;
//struct ProjectileHoming;
//
//class Unit;
///// <summary>
///// 요청된 파라미터에 따라 그에 맞는 ProjectilePool을 선택해 Borrow시켜주는 클래스.
///// </summary>
//class ProjectileSelector : public yunutyEngine::SingletonClass<ProjectileSelector>
//{
//public:
//    std::weak_ptr<Projectile> RequestProjectile(std::weak_ptr<Unit> owner, std::weak_ptr<Unit> opponent, ProjectileType::Enum projectileType);
//};