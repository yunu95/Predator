#pragma once
#include "EnemyController.h"

class HolderController : public EnemyController, public SingletonComponent<HolderController>
{
public:
private:
    virtual coroutine::Coroutine RoutinePerUnit(std::weak_ptr<Unit> unit)override;
};