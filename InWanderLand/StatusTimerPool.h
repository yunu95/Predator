#pragma once
#include <stack>

class Unit;
class StatusTimer;

class StatusTimerPool 
{
private:
	StatusTimerPool();
	~StatusTimerPool();
	static StatusTimerPool* instance;

public:
	static StatusTimerPool* GetInstance();

private:
	std::stack<StatusTimer*> m_statusTimerStack;

public:
	void SetUp();

	StatusTimer* GetStatusTimer();
	void ReturnToPool(StatusTimer* usedTimer);
};

