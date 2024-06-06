#pragma once
#include "YunutyEngine.h"
#include "EnemyController.h"

class AngryMobController : public EnemyController
{
public:
    // 전역적으로 반복되는 루틴
    virtual coroutine::Coroutine RoutineGlobal();
    // 개별 유닛들에게서 반복되는 루틴
    virtual coroutine::Coroutine RoutinePerUnit(std::weak_ptr<Unit> unit);
private:
};