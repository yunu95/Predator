#include "DotweenTimerPool.h"
#include "DotweenTimer.h"

DotweenTimerPool* DotweenTimerPool::dotweenTimerPool = nullptr;

DotweenTimerPool::DotweenTimerPool()
{
	maxSize = 50;
	Expand();
}

DotweenTimerPool::~DotweenTimerPool()
{
	while (!dotweenTimerStack.empty())
	{
		DotweenTimer* tempDotweenTimer = dotweenTimerStack.top();
		dotweenTimerStack.pop();
		delete tempDotweenTimer;
	}
}

DotweenTimerPool* DotweenTimerPool::GetInstance()
{
	if (dotweenTimerPool == nullptr)
		dotweenTimerPool = new DotweenTimerPool;

	return dotweenTimerPool;
}

DotweenTimer* DotweenTimerPool::GetDotweenTimer()
{
	if (dotweenTimerStack.empty())
	{
		Expand();
	}
	DotweenTimer* tempDotweenTimer = dotweenTimerStack.top();
	dotweenTimerStack.pop();

	return tempDotweenTimer;
}

void DotweenTimerPool::Expand()
{
	for (int i = 0; i < maxSize; i++)
	{
		DotweenTimer* dotweenTimer = new DotweenTimer;
		dotweenTimerStack.push(dotweenTimer);
	}
	maxSize += maxSize;
}

void DotweenTimerPool::ReturnDotweenTimer(DotweenTimer* usedTimer)
{
	// 넣기전에 초기화 해주장
	usedTimer->Init();
	dotweenTimerStack.push(usedTimer);
}

