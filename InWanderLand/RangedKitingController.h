#pragma once
#include "EnemyController.h"

class RangedKitingController : public EnemyController, public SingletonComponent<RangedKitingController>
{
public:
    virtual void RegisterUnit(std::weak_ptr<Unit> unit) override;
private:
    virtual coroutine::Coroutine RoutinePerUnit(std::weak_ptr<Unit> unit)override;
};