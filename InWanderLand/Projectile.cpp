#include "InWanderLand.h"
#include "Projectile.h"
#include "ProjectilePool.h"

//void Projectile::Update()
//{
//    UnitAcquisitionSphereCollider::Update();
//    if (traveling)
//    {
//        if (!enemies.empty())
//        {
//            if (!owner.lock()->referenceBlindness.BeingReferenced())
//            {
//                (*enemies.begin())->Damaged(owner, damage, DamageType::Attack);
//            }
//            ProjectilePool<Projectile>::SingleInstance().Return(GetWeakPtr<Projectile>());
//            return;
//        }
//
//        GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition() + speed * Time::GetDeltaTime());
//        SetSpeed(speed + Vector3d::down * GlobalConstant::GetSingletonInstance().pod.gravitySpeed * Time::GetDeltaTime());
//        if (GetTransform()->GetWorldPosition().y < 0)
//        {
//            ProjectilePool<Projectile>::SingleInstance().Return(GetWeakPtr<Projectile>());
//        }
//    }
//}
void Projectile::SetSpeed(Vector3d speed)
{
    this->speed = speed;
    GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(speed, Vector3d::up));
}
//void Projectile::SetSpeed(Vector3d startPosition, Vector3d endPosition, float projectileSpeed)
//{
//    auto delta = endPosition - startPosition;
//    auto deltaY = delta.y;
//    delta.y = 0;
//    float t = delta.Magnitude() / projectileSpeed;
//    float Vy0 = deltaY / t - 0.5f * GlobalConstant::GetSingletonInstance().pod.gravitySpeed * t;
//    SetSpeed(delta.Normalized() * projectileSpeed + Vector3d::up * Vy0);
//}
