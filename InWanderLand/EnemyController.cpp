#include "EnemyController.h"
#include "InWanderLand.h"

void EnemyController::OnContentsStop()
{
    PermanentObservee::OnContentsStop();
    DeleteCoroutine(globalRoutine);
}
void EnemyController::Start()
{
}

void EnemyController::RegisterUnit(std::weak_ptr<Unit> unit)
{
    unitRoutines[unit.lock().get()] = StartCoroutine(RoutinePerUnit(unit));
}

void EnemyController::UnRegisterUnit(std::weak_ptr<Unit> unit)
{
    DeleteCoroutine(unitRoutines[unit.lock().get()]);
    unitRoutines.erase(unit.lock().get());
}

void EnemyController::Update()
{
    if (globalRoutine.expired())
    {
        globalRoutine = StartCoroutine(RoutineGlobal());
    }
    for (auto& each : unitRoutines)
    {
        if (each.second.expired())
        {
            each.second = StartCoroutine(RoutinePerUnit(each.first->GetWeakPtr<Unit>()));
        }
    }
}
