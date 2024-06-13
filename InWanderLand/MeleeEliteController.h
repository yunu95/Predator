#pragma once
#include "EnemyController.h"

class MeleeEliteController : public EnemyController, public SingletonComponent<MeleeEliteController>
{
public:
private:
    virtual coroutine::Coroutine RoutinePerUnit(std::weak_ptr<Unit> unit)override;
};