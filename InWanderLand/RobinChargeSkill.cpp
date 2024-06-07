#include "InWanderLand.h"
#include "RobinChargeSkill.h"

POD_RobinChargeSkill RobinChargeSkill::pod = POD_RobinChargeSkill();



coroutine::Coroutine RobinChargeSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    if (deltaPos.Magnitude() > pod.MaxDistance)
    {
        deltaPos = direction * pod.MaxDistance;
    }
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    owner.lock()->PlayAnimation(UnitAnimType::Rush, true);
    coroutine::ForSeconds forSeconds{ static_cast<float>(deltaPos.Magnitude()) / pod.RushSpeed };
    knockbackCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    knockbackCollider.lock()->SetRadius(pod.RushKnockbackRadius);
    while (forSeconds.Tick())
    {
        currentPos += direction * pod.RushSpeed * Time::GetDeltaTime();
        knockbackCollider.lock()->GetTransform()->SetWorldPosition(currentPos);
        owner.lock()->GetTransform()->SetWorldPosition(currentPos);
        co_await std::suspend_always{};
        for (auto& each : knockbackCollider.lock()->GetEnemies())
        {
            Vector3d delta = pod.RushKnockbackDistance * (each->GetTransform()->GetWorldPosition() - currentPos).Normalized();
            each->KnockBackRelativeVector(delta, pod.RushKnockbackDuration);
            each->Damaged(owner, pod.DamageRush);
        }
    }

    owner.lock()->PlayAnimation(UnitAnimType::Slam);
    owner.lock()->SetDefaultAnimation(UnitAnimType::Idle);
    knockbackCollider.lock()->SetRadius(pod.ImpactKnockbackRadius);
    co_await std::suspend_always{};
    for (auto& each : knockbackCollider.lock()->GetEnemies())
    {
        Vector3d delta = pod.ImpactKnockbackDistance * (each->GetTransform()->GetWorldPosition() - currentPos).Normalized();
        each->KnockBack(each->GetTransform()->GetWorldPosition() + delta, pod.ImpactKnockbackDuration);
        each->Paralyze(pod.ImpactStunDuration);
        each->Damaged(owner, pod.DamageRush);
    }
    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(currentPos);
    co_yield coroutine::WaitForSeconds(0.8);
    owner.lock()->PlayAnimation(UnitAnimType::Idle, true);
    co_yield coroutine::WaitForSeconds(0.2);
    OnInterruption();
    co_return;
}

void RobinChargeSkill::OnInterruption()
{
    knockbackCollider.lock()->SetRadius(0.5);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(knockbackCollider);
}
