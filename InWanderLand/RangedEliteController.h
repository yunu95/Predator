#pragma once
#include "EnemyController.h"

class RangedEliteController : public EnemyController, public SingletonComponent<RangedEliteController>
{
public:
private:
    virtual coroutine::Coroutine RoutinePerUnit(std::weak_ptr<Unit> unit)override;
};