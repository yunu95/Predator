#include "AngryMobController.h"
#include "InWanderLand.h"

// 1초마다 적 유닛들중 하나를 플레이어에게 골고루 분배한다.
coroutine::Coroutine AngryMobController::RoutineGlobal()
{
    return coroutine::Coroutine();
}

coroutine::Coroutine AngryMobController::RoutinePerUnit(std::weak_ptr<Unit> unit)
{
    return coroutine::Coroutine();
}
