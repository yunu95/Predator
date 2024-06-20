#include "InWanderLand.h"
#include "Projectile.h"
#include "ProjectilePool.h"
#include "RTSCam.h"

void Projectile::Update()
{
    UnitAcquisitionSphereCollider::Update();
    if (traveling)
    {
        if (!enemies.empty())
        {
            if (!owner.lock()->referenceBlindness.BeingReferenced())
            {
                (*enemies.begin())->Damaged(owner, damage, GetTransform()->GetWorldPosition());
            }
            ProjectilePool::SingleInstance().Return(GetWeakPtr<Projectile>());
            return;
        }

        if (auto target = homingTarget.lock(); target.get())
        {
            relativePositionFromTarget += speed * Time::GetDeltaTime();
            GetTransform()->SetWorldPosition(target->GetTransform()->GetWorldPosition() + relativePositionFromTarget);
        }
        else
        {
            GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition() + speed * Time::GetDeltaTime());
        }
        if (projectileType == ProjectileType::CURVE)
        {
            SetSpeed(speed + Vector3d::down * GlobalConstant::GetSingletonInstance().pod.gravitySpeed * Time::GetDeltaTime());
        }
        if (GetTransform()->GetWorldPosition().y < 0)
        {
            ProjectilePool::SingleInstance().Return(GetWeakPtr<Projectile>());
        }
        if (projectileType == ProjectileType::DIRECT)
        {
            lifetime -= Time::GetDeltaTime();
            if (lifetime <= 0)
            {
                ProjectilePool::SingleInstance().Return(GetWeakPtr<Projectile>());
            }
        }
    }
}
void Projectile::SetSpeed(Vector3d speed)
{
    this->speed = speed;
    GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(speed, Vector3d::up));
}
void Projectile::OnContentsStop()
{
    ProjectilePool::SingleInstance().Return(GetWeakPtr<Projectile>());
}
void Projectile::SetSpeed(Vector3d startPosition, Vector3d endPosition, float projectileSpeed)
{
    auto delta = endPosition - startPosition;
    switch (projectileType)
    {
    case ProjectileType::CURVE:
    {
        auto deltaY = delta.y;
        delta.y = 0;
        float t = delta.Magnitude() / projectileSpeed;
        float Vy0 = deltaY / t + 0.5f * GlobalConstant::GetSingletonInstance().pod.gravitySpeed * t;
        SetSpeed(delta.Normalized() * projectileSpeed + Vector3d::up * Vy0);
        break;
    }
    case ProjectileType::DIRECT:
    {
        SetSpeed(delta.Normalized() * projectileSpeed);
        lifetime = delta.Magnitude() / projectileSpeed;
        break;
    }
    }
    if (auto target = homingTarget.lock(); target)
    {
        relativePositionFromTarget = startPosition - target->GetTransform()->GetWorldPosition();
    }
}
