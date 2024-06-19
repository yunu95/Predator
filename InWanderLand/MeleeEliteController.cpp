#include "MeleeEliteController.h"
#include "InWanderLand.h"

coroutine::Coroutine MeleeEliteController::RoutinePerUnit(std::weak_ptr<Unit> unit)
{
    wanderUtils::UnitCoroutine::ForSecondsFromUnit waitCoolDown{ unit, EnemySpinAttackSkill::pod.skillCoolTime };

    while (waitCoolDown.Tick())
    {
        co_await std::suspend_always();
    }

	unit.lock()->OrderSkill(EnemySpinAttackSkill{}, unit.lock()->GetTransform()->GetWorldPosition());
}
