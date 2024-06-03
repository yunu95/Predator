#include "InWanderLand.h"
#include "RobinChargeSkill.h"

coroutine::Coroutine RobinChargeSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    if (deltaPos.Magnitude() > GlobalConstant::GetSingletonInstance().pod.robinQSkillMaxDistance)
    {
        deltaPos = direction * GlobalConstant::GetSingletonInstance().pod.robinQSkillMaxDistance;
    }
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    owner.lock()->PlayAnimation(UnitAnimType::Rush, true);
    const application::POD_GlobalConstant& gc = GlobalConstant::GetSingletonInstance().pod;
    coroutine::ForSeconds forSeconds{ static_cast<float>(deltaPos.Magnitude()) / gc.robinQSkillRushSpeed };
    auto rushCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    rushCollider.lock()->SetRadius(gc.robinQSkillRushKnockbackRadius);
    while (forSeconds.Tick())
    {
        currentPos += direction * gc.robinQSkillRushSpeed * Time::GetDeltaTime();
        rushCollider.lock()->GetTransform()->SetWorldPosition(currentPos);
        owner.lock()->GetTransform()->SetWorldPosition(currentPos);
        co_await std::suspend_always{};
        for (auto& each : rushCollider.lock()->GetEnemies())
        {
            Vector3d delta = gc.robinQSkillRushKnockbackDistance * (each->GetTransform()->GetWorldPosition() - currentPos).Normalized();
            each->KnockBack(each->GetTransform()->GetWorldPosition() + delta, gc.robinQSkillRushKnockbackDuration);
            each->Damaged(owner, gc.robinQSkillDamageRush);
        }
    }

    owner.lock()->PlayAnimation(UnitAnimType::Slam, false);
    rushCollider.lock()->SetRadius(gc.robinQSkillImpactKnockbackRadius);
    co_await std::suspend_always{};
    for (auto& each : rushCollider.lock()->GetEnemies())
    {
        Vector3d delta = gc.robinQSkillImpactKnockbackDistance * (each->GetTransform()->GetWorldPosition() - currentPos).Normalized();
        each->KnockBack(each->GetTransform()->GetWorldPosition() + delta, gc.robinQSkillImpactKnockbackDuration);
        each->Paralyze(gc.robinQSkillImpactStunDuration);
        each->Damaged(owner, gc.robinQSkillDamageRush);
    }
    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(currentPos);
    co_yield coroutine::WaitForSeconds(0.2);
    co_return;
}
