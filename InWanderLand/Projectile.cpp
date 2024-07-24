#include "InWanderLand.h"
#include "Projectile.h"
#include "ProjectilePool.h"
#include "RTSCam.h"

void Projectile::Update()
{
    UnitAcquisitionSphereCollider::Update();

    previousTransform = GetTransform();

    if (traveling)
    {
        if (!enemies.empty())
        {
            if (owner.lock()->referenceBlindness.BeingReferenced())
            {
                fbxObject->SetSelfActive(true);
                ProjectilePool::SingleInstance().Return(GetWeakPtr<Projectile>());
                owner.lock()->ShowMissedUI();
            }
            else
            {
                if (fbxObject->getName() == "VFX_Ursula_Attack_1")
                {
                    SFXManager::PlaySoundfile3D(wanderResources::GetSoundPath(EffectSoundType::Enum::Projectile_Ursula), (*enemies.begin())->GetTransform()->GetWorldPosition());
                }
                else if (fbxObject->getName() == "SM_Pie")
                {
                    SFXManager::PlaySoundfile3D(wanderResources::GetSoundPath(EffectSoundType::Enum::Projectile_Hansel), (*enemies.begin())->GetTransform()->GetWorldPosition());
                }
                else if (fbxObject->getName() == "Spear")
                {
                    SFXManager::PlaySoundfile3D(wanderResources::GetSoundPath(EffectSoundType::Enum::Projectile_RangedEnemy), (*enemies.begin())->GetTransform()->GetWorldPosition());
                }

                (*enemies.begin())->Damaged(owner, damage, damageType);
                if (wanderResources::FindVFXMap(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Damaged))
                {
                    fbxObject->SetSelfActive(false);
                    (*enemies.begin())->StartCoroutine(ProjectileEffectCoroutine((*enemies.begin())->GetWeakPtr<Unit>()))
                        .lock()->PushDestroyCallBack([this]()
                            {
                                if (!damagedVFX.expired())
                                {
                                    FBXPool::Instance().Return(damagedVFX);
                                }
                                fbxObject->SetSelfActive(true);
                                ProjectilePool::SingleInstance().Return(GetWeakPtr<Projectile>());
                            });
                }
                else
                {
                    ProjectilePool::SingleInstance().Return(GetWeakPtr<Projectile>());
                }
                traveling = false;
            }
            return;
        }

        if (auto target = homingTarget.lock(); target.get())
        {
            if (!homingTarget.lock()->IsAlive())
            {
                // 쫓던 유닛이 사망한 경우 해당 유닛의 위치를 목표 지점으로 설정, 도달할 경우 이펙트 없이 터뜨린다.
                targetDeadPosition = homingTarget.lock()->GetTransform()->GetWorldPosition() + offsetTargetPos;
                homingTarget.reset();
            }
            else
            {
                relativePositionFromTarget += speed * Time::GetDeltaTime();
                GetTransform()->SetWorldPosition(target->GetTransform()->GetWorldPosition() + relativePositionFromTarget);
                GetTransform()->SetLocalRotation(Quaternion::MakeWithForwardUp(speed, speed.up));
            }
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
            if (wanderResources::FindVFXMap(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Damaged))
            {
                /// 투사체가 터지는 이펙트가 존재할 경우.
                ExplodeAtCurrentPosition();
            }
            else
            {
                ProjectilePool::SingleInstance().Return(GetWeakPtr<Projectile>());
            }
            traveling = false;
        }
        if (projectileType == ProjectileType::DIRECT)
        {
            lifetime -= Time::GetDeltaTime();
            if (lifetime <= 0)
            {
                ProjectilePool::SingleInstance().Return(GetWeakPtr<Projectile>());
            }
        }
        if (targetDeadPosition != Vector3d::zero)
        {
            Vector3d startPosition = Vector3d(GetTransform()->GetWorldPosition().x,
                targetDeadPosition.y, GetTransform()->GetWorldPosition().z);

            Vector3d directionVector = (targetDeadPosition - startPosition).Normalized();

            if (beforeDirectionVector != Vector3d::zero)
            {
                float dotProducted = Vector3d::Dot(beforeDirectionVector, directionVector);
                float multipledLength = directionVector.Magnitude() * beforeDirectionVector.Magnitude();
                float angle = dotProducted / multipledLength;
                if (angle < 0)
                {
                    ExplodeAtCurrentPosition(false);
                }
            }
            beforeDirectionVector = directionVector;
        }
        else
        {
            int a = 0;
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

void Projectile::ExplodeAtCurrentPosition(bool withEffectOn)
{
    fbxObject->SetSelfActive(false);
    traveling = false;

    if (owner.lock()->GetUnitTemplateData().pod.skinnedFBXName != "SKM_Monster2" && withEffectOn)
    {
        StartCoroutine(ProjectileEffectCoroutine(std::weak_ptr<Unit>())).lock()->PushDestroyCallBack([this]()
            {
                if (!damagedVFX.expired())
                {
                    FBXPool::Instance().Return(damagedVFX);
                }
                fbxObject->SetSelfActive(true);
                ProjectilePool::SingleInstance().Return(GetWeakPtr<Projectile>());
            });
    }
    else
    {
        targetDeadPosition = Vector3d::zero;
        ProjectilePool::SingleInstance().Return(GetWeakPtr<Projectile>());
    }
}

coroutine::Coroutine Projectile::ProjectileEffectCoroutine(std::weak_ptr<Unit> opponent)
{
    damagedVFX = wanderResources::GetVFX(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Damaged);
    co_await std::suspend_always{};
    auto vfxAnimator = damagedVFX.lock()->AcquireVFXAnimator();
    vfxAnimator.lock()->SetAutoActiveFalse();
    vfxAnimator.lock()->Init();
    vfxAnimator.lock()->Play();
    //direction *= -1;
    //damagedVFX.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos + direction * 2);

    if (GetTransform()->GetWorldPosition().y < 0)
    {
        damagedVFX.lock()->GetGameObject()->GetTransform()->SetWorldPosition(Vector3d(GetTransform()->GetWorldPosition().x, 0.0f, GetTransform()->GetWorldPosition().z));

        auto temp = GetTransform()->GetLocalRotation();
        auto euler = temp.Euler();
        euler.x = 0;
        
        damagedVFX.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion{ euler });
    }
    else
    {
        damagedVFX.lock()->GetGameObject()->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
        auto temp = GetTransform()->GetLocalRotation();
        auto euler = temp.Euler();
        euler.x += 90.0f;

        damagedVFX.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion{ euler });
    }

    //damagedVFX.lock()->GetGameObject()->GetTransform()->SetWorldRotation(direction);
    //damagedVFX.lock()->GetGameObject()->GetTransform()->SetWorldScale(GetTransform()->GetWorldScale());

    if (opponent.expired())
    {
        while (!vfxAnimator.lock()->IsDone())
        {
            co_await std::suspend_always{};
        }
    }
    else
    {
        auto relativePos = GetTransform()->GetWorldPosition() - opponent.lock()->GetTransform()->GetWorldPosition();
        auto prevRot = opponent.lock()->QuaternionToEastAngle(opponent.lock()->GetTransform()->GetWorldRotation());

        while (!vfxAnimator.lock()->IsDone())
        {
            damagedVFX.lock()->GetGameObject()->GetTransform()->SetWorldPosition(opponent.lock()->GetTransform()->GetWorldPosition() + relativePos);
            auto gc = GlobalConstant::GetSingletonInstance().pod;

            //damagedVFX.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion{ Vector3d(gc.tempProRotX, gc.tempProRotY, gc.tempProRotZ) });
            //damagedVFX.lock()->GetGameObject()->GetTransform()->SetWorldRotation(damagedVFX.lock()->GetGameObject()->GetTransform()->GetWorldRotation() * Quaternion(Vector3d{ 0, prevRot - QuaternionToEastAngle(GetTransform()->GetWorldRotation()), 0}));
            //prevRot = QuaternionToEastAngle(GetTransform()->GetWorldRotation());
            co_await std::suspend_always{};
        }
    }

    co_return;
}

void Projectile::SetDamageType(DamageType damageType)
{
    this->damageType = damageType;
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
        lifetime = delta.Magnitude() / projectileSpeed + 100.0f;
        break;
    }
    }
    if (auto target = homingTarget.lock(); target)
    {
        relativePositionFromTarget = startPosition - target->GetTransform()->GetWorldPosition();
    }
}
