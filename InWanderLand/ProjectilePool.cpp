#include "Projectile.h"
#include "ProjectilePool.h"

// Borrow 함수는 먼저 해당 fbx 이름에 맞는 풀이 존재하는지 확인하고, 없으면 새로 만든다.
// 해당 풀이 있다면 
std::weak_ptr<Projectile> ProjectilePool::Borrow(string fbxname)
{
    if (!poolsByFBX.contains(fbxname))
    {
        auto pool = std::make_shared<PoolByMesh>();
        pool->fbxname = fbxname;
        poolsByFBX[fbxname] = pool;
    }
    auto ret = poolsByFBX[fbxname]->Borrow();
    ret.lock()->traveling = true;
    return ret;
}

void ProjectilePool::Return(std::weak_ptr<Projectile> projectile)
{
    projectile.lock()->traveling = false;
    poolsByProjectile[projectile].lock()->Return(projectile);
}

void ProjectilePool::PoolByMesh::ObjectInitializer(std::weak_ptr<Projectile> unit)
{
    auto gameObj = Scene::getCurrentScene()->AddGameObjectFromFBX(fbxname);
    std::weak_ptr<Projectile> projectile = gameObj->AddComponentAsWeakPtr<Projectile>();
    ProjectilePool::SingleInstance().poolsByProjectile[projectile] = ProjectilePool::SingleInstance().poolsByFBX[fbxname];
    gameObj->AddComponentAsWeakPtr<physics::SphereCollider>().lock()->SetRadius(0.01);
}
