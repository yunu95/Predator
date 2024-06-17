#pragma once
#include "WanderUtils.h"
#include "UnitController.h"

class Unit;
class EnemyController : public UnitController
{
public:
    static bool IsPreempted();

    virtual void RegisterUnit(std::weak_ptr<Unit> unit) override;
    virtual void UnRegisterUnit(std::weak_ptr<Unit> unit) override;
protected:
    virtual void Start() override;
    virtual void OnContentsStop() override;
    virtual void Update() override;
    // 특정 지점으로부터 가까이 있는 유닛들을 가까운 순서대로 반환
    //const std::vector<std::weak_ptr<Unit>>& GetClosestUnits(const Vector3d& from std::weak_ptr<Unit> unit);
    // 전역적으로 반복되는 루틴
    virtual coroutine::Coroutine RoutineGlobal() { co_yield coroutine::WaitForSeconds{ 10 }; };
    // 개별 유닛들에게서 반복되는 루틴
    virtual coroutine::Coroutine RoutinePerUnit(std::weak_ptr<Unit> unit) { co_yield coroutine::WaitForSeconds{ 10 }; };
    std::weak_ptr<coroutine::Coroutine> globalRoutine;
    std::unordered_map<Unit*, std::weak_ptr<coroutine::Coroutine>> unitRoutines;

private:
    static std::unordered_set<std::shared_ptr<Reference::Guard>> enemyDeathBlockRefSet;
};