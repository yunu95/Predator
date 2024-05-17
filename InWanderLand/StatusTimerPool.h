#pragma once
#include <stack>
#include "GameObjectPool.h"
#include "StatusTimer.h"
#include "PermanentObservee.h"

class Unit;
class StatusTimer;

class StatusTimerPool : public GameObjectPool<StatusTimer>, public SingletonComponent<StatusTimerPool>, public Component, public PermanentObservee
{
public:
	virtual void ObjectInitializer(StatusTimer* timer) override
	{

	}
	virtual void Start() override;
	virtual void OnContentsStop() override;
	virtual Component* GetComponent() override { return this; }
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

