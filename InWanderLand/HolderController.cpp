#include "HolderController.h"
#include "InWanderLand.h"

coroutine::Coroutine HolderController::RoutinePerUnit(std::weak_ptr<Unit> unit)
{
    unit.lock()->OrderHold();
    co_return;
}