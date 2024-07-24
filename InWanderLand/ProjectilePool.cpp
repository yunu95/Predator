#include "ProjectilePool.h"
#include "Projectile.h"

std::weak_ptr<Projectile> ProjectilePool::Borrow(std::weak_ptr<Unit> owner, std::weak_ptr<Unit> opponent)
{
    auto ownerPtr = owner.lock();
    const std::string& fbxname = ownerPtr->GetUnitTemplateData().pod.projectile_staticFBXName;
    if (!poolsByFBX.contains(fbxname))
    {
        std::shared_ptr<PoolByMesh> pool = std::make_shared<PoolByMesh>();
        pool->fbxname = fbxname;
        poolsByFBX[fbxname] = pool;
    }
    std::weak_ptr<Projectile> ret = poolsByFBX[fbxname]->Borrow();
    auto projectile = ret.lock();
    projectile->owner = owner;
    projectile->targetDeadPosition = Vector3d::zero;
    projectile->beforeDirectionVector = Vector3d::zero;
    const auto& ownerTD = ownerPtr->GetUnitTemplateData().pod;
    const auto& v = ownerTD.projectileOffset;
    DirectX::XMVECTOR offset = DirectX::XMVectorSet(v.x, v.y, v.z, 1);
    auto yunuWtm = ownerPtr->GetTransform()->GetWorldTM();
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
    projectile->damage = ownerPtr->GetUnitTemplateData().pod.m_autoAttackDamage * (1 + ownerPtr->multiplierDamage) + ownerPtr->adderAttackDamage;
    projectile->traveling = true;
    auto opponentPtr = opponent.lock();
    auto destination = opponentPtr->GetRandomPositionInsideCapsuleCollider();
    projectile->offsetTargetPos = destination - opponentPtr->GetTransform()->GetWorldPosition();
    float distanceToOpponent = (-projectile->GetTransform()->GetWorldPosition() + destination).Magnitude();
    float noise = math::Random::GetRandomFloat(0.0f, ownerTD.projectileTargetNoise * distanceToOpponent) - opponentPtr->GetUnitTemplateData().pod.collisionSize;
    if (noise > 0)
    {
        destination += Quaternion{ Vector3d{0, math::Random::GetRandomFloat(180), 0} }.Forward() * noise;
    }
    projectile->SetSpeed({ offset.m128_f32[0],offset.m128_f32[1],offset.m128_f32[2] }, destination, ownerPtr->GetProjectileSpeed());
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
    projectile.lock()->fbxObject = gameObj;

    if (fbxname != "VFX_Ursula_Attack_1")
        gameObj->GetTransform()->SetLocalRotation(Vector3d(90, 0, 0));

    ProjectilePool::SingleInstance().poolsByProjectile[projectile.lock().get()] = ProjectilePool::SingleInstance().poolsByFBX.at(fbxname);
    projectile.lock()->SetRadius(0.0001);
}
