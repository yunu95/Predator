#include "Projectile.h"
#include "ProjectilePool.h"

std::weak_ptr<Projectile> ProjectilePool::Borrow(string fbxname)
{
    if (!poolsByFBX.contains(fbxname))
    {
        auto pool = make_shared<PoolByMesh>();
        pool->fbxname = fbxname;
        poolsByFBX[fbxname] = pool;
    }
    auto ret = poolsByFBX[fbxname]->Borrow();
    ret.lock()->traveling = true;
    return ret;
}

void ProjectilePool::Return(std::weak_ptr<Projectile> projectile)
{
    std::unordered_map<std::weak_ptr<Projectile>, std::weak_ptr<PoolByMesh>> poolsByProjectile;
    projectile.lock()->traveling = false;
    poolsByProjectile[projectile].lock()->Return(projectile);
}

void ProjectilePool::PoolByMesh::ObjectInitializer(std::weak_ptr<Projectile> unit)
{
    auto gameObj{ Scene::getCurrentScene()->AddGameObjectFromFBX(fbxname) };
    std::weak_ptr<Projectile> projectile = gameObj->AddComponentAsWeakPtr<Projectile>();
    ProjectilePool::SingleInstance().poolsByProjectile[projectile] = ProjectilePool::SingleInstance().poolsByFBX[fbxname];
    gameObj->AddComponentAsWeakPtr<physics::SphereCollider>().lock()->SetRadius(0.01);
}
