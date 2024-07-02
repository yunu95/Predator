#include "RangedApproachingController.h"
#include "InWanderLand.h"

void RangedApproachingController::RegisterUnit(std::weak_ptr<Unit> unit)
{
}

coroutine::Coroutine RangedApproachingController::RoutinePerUnit(std::weak_ptr<Unit> unit)
{
    const auto& gc = GlobalConstant::GetSingletonInstance().pod;
    auto unitPtr = unit.lock();
    auto attackRangePtr = unitPtr->attackRange.lock();
    if (attackRangePtr->GetRadius() > gc.enemyApproachingDecayRate)
    {
        attackRangePtr->SetRadius(attackRangePtr->GetRadius() * gc.enemyApproachingDecayRate);
    }
    co_yield coroutine::WaitForSeconds{ gc.enemyApproachingDecayCooltime };
    co_return;
}
