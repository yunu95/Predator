#pragma once
#include "EnemyController.h"

class RangedApproachingController : public EnemyController, public SingletonComponent<RangedApproachingController>
{
public:
    virtual void RegisterUnit(std::weak_ptr<Unit> unit) override;
private:
    virtual coroutine::Coroutine RoutinePerUnit(std::weak_ptr<Unit> unit)override;
};