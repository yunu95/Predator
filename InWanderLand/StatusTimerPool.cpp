#include "InWanderLand.h"
#include "StatusTimerPool.h"
#include "StatusTimer.h"
#include "YunutyEngine.h"

StatusTimerPool* StatusTimerPool::instance = nullptr;

StatusTimerPool::StatusTimerPool()
{
	instance = this;
}

StatusTimerPool::~StatusTimerPool()
{
	delete instance;
}

StatusTimerPool* StatusTimerPool::GetInstance()
{
	if (instance == nullptr)
		instance = new StatusTimerPool;
	return instance;
}

StatusTimer* StatusTimerPool::GetStatusTimer()
{
	if (m_statusTimerStack.empty())
	{
		SetUp();
	}

	StatusTimer* deliverPredictedTimer = m_statusTimerStack.top();

	m_statusTimerStack.pop();

	return deliverPredictedTimer;
}

void StatusTimerPool::ReturnToPool(StatusTimer* usedTimer)
{
	m_statusTimerStack.push(usedTimer);
}

void StatusTimerPool::SetUp()
{
	for (int i = 0; i < 10; i++)
	{
		m_statusTimerStack.push(yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<StatusTimer>());
	}
}
