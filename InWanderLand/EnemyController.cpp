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
//
//const std::vector<std::weak_ptr<Unit>>& EnemyController::GetClosestUnits(const Vector3d& from std::weak_ptr<Unit> unit)
//{
//    static std::vector<std::weak_ptr<Unit>> sortedUnits;
//    sortedUnits.clear();
//    std::transform(unitRoutines.begin(), unitRoutines.end(), std::back_inserter(sortedUnits), [](const std::pair<Unit*, CoroutineHandle>& each) { return each.first->GetWeakPtr<Unit>(); });
//    std::sort(sortedUnits.begin(), sortedUnits.end(), [from](const std::weak_ptr<Unit>& a, const std::weak_ptr<Unit>& b) { return (a.lock()->GetTransform()->GetWorldPosition() - from).() < (a.lock()->GetTransform()->GetWorldPosition() - from).sqrMagnitude(); });
//    return sortedUnits;
//}

