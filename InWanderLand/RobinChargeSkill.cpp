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
    if (deltaPos.Magnitude() > pod.maxDistance)
    {
        deltaPos = direction * pod.maxDistance;
    }
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    owner.lock()->PlayAnimation(UnitAnimType::Rush, true);
    coroutine::ForSeconds forSeconds{ static_cast<float>(deltaPos.Magnitude()) / pod.rushSpeed };
    knockbackCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    knockbackCollider.lock()->SetRadius(pod.rushKnockbackRadius);
    while (forSeconds.Tick())
    {
        currentPos += direction * pod.rushSpeed * Time::GetDeltaTime();
        knockbackCollider.lock()->GetTransform()->SetWorldPosition(currentPos);
        owner.lock()->GetTransform()->SetWorldPosition(currentPos);
        co_await std::suspend_always{};
        for (auto& each : knockbackCollider.lock()->GetEnemies())
        {
            Vector3d delta = pod.rushKnockbackDistance * (each->GetTransform()->GetWorldPosition() - currentPos).Normalized();
            each->KnockBackRelativeVector(delta, pod.rushKnockbackDuration);
            each->Damaged(owner, pod.damageRush);
        }
    }

    owner.lock()->PlayAnimation(UnitAnimType::Slam);
    owner.lock()->SetDefaultAnimation(UnitAnimType::Idle);
    knockbackCollider.lock()->SetRadius(pod.impactKnockbackRadius);
    co_await std::suspend_always{};
    for (auto& each : knockbackCollider.lock()->GetEnemies())
    {
        Vector3d delta = pod.impactKnockbackDistance * (each->GetTransform()->GetWorldPosition() - currentPos).Normalized();
        each->KnockBack(each->GetTransform()->GetWorldPosition() + delta, pod.impactKnockbackDuration);
        each->Paralyze(pod.impactStunDuration);
        each->Damaged(owner, pod.damageRush);
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
