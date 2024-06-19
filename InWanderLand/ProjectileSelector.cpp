//#include "ProjectileSelector.h"
//#include "InWanderLand.h"
//#include "ProjectilePool.h"
//
//std::weak_ptr<Projectile> ProjectileSelector::RequestProjectile(std::weak_ptr<Unit> owner, std::weak_ptr<Unit> opponent, ProjectileType::Enum projectileType, ProjectileHoming::Enum projectileHoming)
//{
//    std::weak_ptr<Projectile> projectile;
//    if (projectileType == ProjectileType::DIRECT)
//    {
//        if (projectileHoming == ProjectileHoming::NO)
//            projectile = ProjectilePool<DirectProjectile>::SingleInstance().Borrow(owner, opponent);
//        else
//            projectile = ProjectilePool<DirectHomingProjectile>::SingleInstance().Borrow(owner, opponent);
//    }
//    else
//    {
//        if (projectileHoming == ProjectileHoming::NO)
//            projectile = ProjectilePool<CurveProjectile>::SingleInstance().Borrow(owner, opponent);
//        else
//            projectile = ProjectilePool<CurveHomingProjectile>::SingleInstance().Borrow(owner, opponent);
//    }
//
//    return projectile;
//}
