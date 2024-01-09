#pragma once
#include <stack>
#include "YunutyEngine.h"

/// <summary>
/// DO함수를 쓸 때 마다 new 해주고,
/// 다 쓰고 나서 delete 해주는 것은 효율적이지 않다.
/// 미리 DotweenTimer를 50개 정도 생성해주고 가져다 써보자.
/// </summary>

class DotweenTimer;

class DotweenTimerPool
{
private:
	std::stack<DotweenTimer*> dotweenTimerStack;
	int maxSize;
	static DotweenTimerPool* dotweenTimerPool;

	DotweenTimerPool();
	~DotweenTimerPool();
	DotweenTimerPool(const DotweenTimerPool& ref) {}
	DotweenTimerPool& operator=(const DotweenTimerPool& ref) {}

public:
	static DotweenTimerPool* GetInstance();

	DotweenTimer* GetDotweenTimer();
	void Expand();
	void ReturnDotweenTimer(DotweenTimer* usedTimer);
};

