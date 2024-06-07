#include "RangedEliteController.h"
#include "InWanderLand.h"

coroutine::Coroutine RangedEliteController::RoutinePerUnit(std::weak_ptr<Unit> unit)
{
    if (auto targetUnit = unit.lock()->GetAttackTarget(); !targetUnit.expired())
    {
        if (Unit::Distance(unit, targetUnit) > EnemyImpaleSkill::pod.impaleSkillRange * 0.5f)
        {
            unit.lock()->OrderMove(targetUnit.lock()->GetTransform()->GetWorldPosition() + Unit::FromTo(unit, targetUnit).Normalized() * EnemyImpaleSkill::pod.impaleSkillRange * 0.5f);
        }
        co_yield coroutine::WaitForSeconds{ EnemyImpaleSkill::pod.impaleSkillApproachingTime };
        unit.lock()->OrderSkill(EnemyImpaleSkill{}, targetUnit.lock()->GetTransform()->GetWorldPosition());
    }
    co_yield coroutine::WaitForSeconds{ EnemyImpaleSkill::pod.impaleSkillCoolTime };
}
