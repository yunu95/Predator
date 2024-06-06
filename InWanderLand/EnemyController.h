#pragma once
#include "UnitController.h"

class EnemyController : public UnitController
{
public:
protected:
    virtual void Start() override;
    virtual void OnContentsStop() override;
    virtual void RegisterUnit(std::weak_ptr<Unit> unit) override;
    virtual void UnRegisterUnit(std::weak_ptr<Unit> unit) override;
    virtual void Update() override;
    // 전역적으로 반복되는 루틴
    virtual coroutine::Coroutine RoutineGlobal() {};
    // 개별 유닛들에게서 반복되는 루틴
    virtual coroutine::Coroutine RoutinePerUnit(std::weak_ptr<Unit> unit) {};
private:
    std::weak_ptr<coroutine::Coroutine> globalRoutine;
    std::unordered_map<Unit*, std::weak_ptr<coroutine::Coroutine>> unitRoutines;
};