#pragma once
#include <stack>
#include "GameObjectPool.h"
#include "StatusTimer.h"

class Unit;
class StatusTimer;

class StatusTimerPool : public GameObjectPool<StatusTimer>, public SingletonClass<StatusTimerPool>
{
public:
	virtual void ObjectInitializer(StatusTimer* projectile) override
	{

	}
};

