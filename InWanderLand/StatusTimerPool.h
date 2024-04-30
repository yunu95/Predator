#pragma once
#include <stack>
#include "GameObjectPool.h"
#include "StatusTimer.h"
#include "ContentsObservee.h"

class Unit;
class StatusTimer;

class StatusTimerPool : public GameObjectPool<StatusTimer>, public Component, public SingletonComponent<StatusTimerPool>, public ContentsObservee
{
public:
	virtual void ObjectInitializer(StatusTimer* timer) override
	{

	}
	virtual void Start() override;
	virtual void PlayFunction() override;
	virtual void StopFunction() override;
};

void StatusTimerPool::Start()
{
	isSingletonComponent = true;
}

void StatusTimerPool::PlayFunction()
{
	this->SetActive(true);
	if (isOncePaused)
	{
		Start();
	}
}

void StatusTimerPool::StopFunction()
{
	poolObjects.clear();
	expendableObjects.clear();
}

