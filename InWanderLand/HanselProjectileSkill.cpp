#include "InWanderLand.h"
#include "HanselProjectileSkill.h"

coroutine::Coroutine HanselProjectileSkill::ThrowingPie()
{
    const application::POD_GlobalConstant& gc = GlobalConstant::GetSingletonInstance().pod;

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    float hanselESkillProjectileSpeed = 5.0f;
    float hanselESkillProjectileRadius = 4.0f;

    coroutine::ForSeconds forSeconds{ static_cast<float>(deltaPos.Magnitude()) / hanselESkillProjectileSpeed };
    pieCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    pieCollider.lock()->SetRadius(hanselESkillProjectileRadius);

    pieCollider.lock()->GetTransform()->SetWorldRotation(direction);

    while (forSeconds.Tick())
    {
        currentPos += direction * hanselESkillProjectileSpeed * Time::GetDeltaTime();
        pieCollider.lock()->GetTransform()->SetWorldPosition(currentPos);
        co_await std::suspend_always{};
        for (auto& each : pieCollider.lock()->GetEnemies())
        {
            if (onceCollidedUnits.find(each) == onceCollidedUnits.end())
            {
                /// 충돌한 적군에게는 실명(디버프)을 부여합니다.
                each->Damaged(owner, 20);
                onceCollidedUnits.insert(each);
            }
        }

        for (auto& each : pieCollider.lock()->GetFriends())
        {
            if (onceCollidedUnits.find(each) == onceCollidedUnits.end())
            {
                /// 충돌한 적군에게는 실명(디버프)을 부여합니다.
                each->Damaged(owner, 20);
                onceCollidedUnits.insert(each);
            }
        }
    }

    pieCollider.lock()->SetRadius(0.5);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(pieCollider);
    onceCollidedUnits.clear();
    co_return;
}

coroutine::Coroutine HanselProjectileSkill::operator()()
{
    const application::POD_GlobalConstant& gc = GlobalConstant::GetSingletonInstance().pod;

    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    owner.lock()->SetDesiredRotation(direction);

    float hanselESkillThrowingStartDelay = 2.0f;

    coroutine::ForSeconds forThrowingSeconds{ hanselESkillThrowingStartDelay };

    owner.lock()->PlayAnimation(UnitAnimType::Skill2, true);

    while (forThrowingSeconds.Tick())
    {
        co_await std::suspend_always{};
    }
    owner.lock()->StartCoroutine(ThrowingPie());

    coroutine::ForSeconds forIdleSeconds{ 0.3f };
    while (forIdleSeconds.Tick()) { co_await std::suspend_always{}; }
    owner.lock()->PlayAnimation(UnitAnimType::Idle);
    co_yield coroutine::WaitForSeconds{ 0.3f };

    co_return;
}

void HanselProjectileSkill::OnInterruption()
{

}
