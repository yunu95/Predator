#include "RangedEliteController.h"
#include "InWanderLand.h"

coroutine::Coroutine RangedEliteController::RoutinePerUnit(std::weak_ptr<Unit> unit)
{
    if (auto targetUnit = unit.lock()->GetAttackTarget(); !targetUnit.expired())
    {
        /*if (Unit::Distance(unit, targetUnit) > EnemyImpaleSkill::pod.impaleSkillRange * 0.5f)
        {
            unit.lock()->OrderMove(targetUnit.lock()->GetTransform()->GetWorldPosition() + Unit::FromTo(unit, targetUnit).Normalized() * EnemyImpaleSkill::pod.impaleSkillRange * 0.5f);
        }
        co_yield coroutine::WaitForSeconds{ EnemyImpaleSkill::pod.impaleSkillApproachingTime };*/
        wanderUtils::UnitCoroutine::ForSecondsFromUnit waitCoolDown{ unit, EnemyImpaleSkill::pod.impaleSkillCoolTime };
        while (waitCoolDown.Tick())
        {
            co_await std::suspend_always();
        }
        auto skillDir = (targetUnit.lock()->GetTransform()->GetWorldPosition() - unit.lock()->GetTransform()->GetWorldPosition()).Normalized();
        unit.lock()->OrderSkill(EnemyImpaleSkill{}, unit.lock()->GetTransform()->GetWorldPosition() + skillDir);
        bool skillDone = false;
        unit.lock()->OnStateExitCallback()[UnitBehaviourTree::SkillOnGoing].AddVolatileCallback([&]()
            {
                skillDone = true;
            });
        while (!skillDone)
        {
            co_await std::suspend_always();
        }
    }
}
