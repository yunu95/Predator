#include "ProjectilePool.h"
#include "Projectile.h"

std::weak_ptr<Projectile> ProjectilePool::Borrow(std::weak_ptr<Unit> owner, std::weak_ptr<Unit> opponent)
{
    const std::string& fbxname = owner.lock()->GetUnitTemplateData().pod.projectile_staticFBXName;
    if (!poolsByFBX.contains(fbxname))
    {
        std::shared_ptr<PoolByMesh> pool = std::make_shared<PoolByMesh>();
        pool->fbxname = fbxname;
        poolsByFBX[fbxname] = pool;
    }
    std::weak_ptr<Projectile> ret = poolsByFBX[fbxname]->Borrow();
    auto projectile = ret.lock();
    projectile->owner = owner;
    const auto& ownerTD = owner.lock()->GetUnitTemplateData().pod;
    const auto& v = ownerTD.projectileOffset;
    DirectX::XMVECTOR offset = DirectX::XMVectorSet(v.x, v.y, v.z, 1);
    auto yunuWtm = owner.lock()->GetTransform()->GetWorldTM();
    DirectX::XMMATRIX wtm = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&yunuWtm));
    offset = DirectX::XMVector4Transform(offset, wtm);
    projectile->GetTransform()->SetWorldPosition({ offset.m128_f32[0],offset.m128_f32[1],offset.m128_f32[2] });
    projectile->opponentUnit = opponent;
    if (ownerTD.isProjectileHoming)
    {
        projectile->homingTarget = opponent;
    }
    else
    {
        projectile->homingTarget.reset();
    }
    projectile->projectileType = (ProjectileType::Enum)ownerTD.projectileType.enumValue;
    projectile->damage = owner.lock()->GetUnitTemplateData().pod.m_autoAttackDamage + owner.lock()->adderAttackDamage;
    projectile->traveling = true;
    projectile->SetSpeed({ offset.m128_f32[0],offset.m128_f32[1],offset.m128_f32[2] }, opponent.lock()->GetRandomPositionInsideCapsuleCollider(), ownerTD.projectileSpeed);
    projectile->GetTransform()->SetLocalScale(Vector3d::one * ownerTD.projectile_scale);

    return ret;
}

void ProjectilePool::Return(std::weak_ptr<Projectile> projectile)
{
    if (projectile.expired())
    {
        return;
    }
    projectile.lock()->traveling = false;
    poolsByProjectile.at(projectile.lock().get()).lock()->Return(projectile);
}

void ProjectilePool::PoolByMesh::ObjectInitializer(std::weak_ptr<Projectile> projectile)
{
    auto gameObj = Scene::getCurrentScene()->AddGameObjectFromFBX(fbxname);
    gameObj->SetParent(projectile.lock()->GetGameObject());

    if (fbxname != "VFX_Ursula_Attack_1")
        gameObj->GetTransform()->SetLocalRotation(Vector3d(90, 0, 0));

    ProjectilePool::SingleInstance().poolsByProjectile[projectile.lock().get()] = ProjectilePool::SingleInstance().poolsByFBX.at(fbxname);
    projectile.lock()->SetRadius(0.0001);
}
