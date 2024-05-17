#pragma once
#include <stack>
#include "GameObjectPool.h"
#include "StatusTimer.h"
#include "PermanentObservee.h"

class Unit;
class StatusTimer;

class StatusTimerPool : public GameObjectPool<StatusTimer>, public SingletonComponent<StatusTimerPool>, public PermanentObservee
{
public:
	virtual void ObjectInitializer(StatusTimer* timer) override
	{

	}
	virtual void Start() override;
	virtual void OnContentsStop() override;
};

void StatusTimerPool::Start()
{
}

void StatusTimerPool::OnContentsStop()
{
	this->SetActive(false);
	poolObjects.clear();
	expendableObjects.clear();
}

