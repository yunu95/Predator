#include "InWanderLand.h"
#include "DotweenTimer.h"
#include "YunutyEngine.h"
#include "DotweenTimerPool.h"

void DotweenTimer::Init()
{
	m_xValue = 0;
	m_yValue = 0;
	m_zValue = 0;

	easeTime = 0;
	elapsed = 0;
	duration = 0;
	delay = 0;

	movingDistanceX = 0;
	movingDistanceY = 0;
	movingDistanceZ = 0;

	speed = 0;

	normalizedDistanceX = 0;
	normalizedDistanceY = 0;
	normalizedDistanceZ = 0;

	finalXvalue = 0;
	finalYvalue = 0;
	finalZvalue = 0;

	accumulated = 0;		// 누적 회전량

	isActive = false;
	isRepeat = false;
	isDone = false;

	m_ease = EaseInSine;
	onUpdate = []() {};
	onCompleteFunction = []() {};
	onExpiration = []() {};
}

// Start를 부르면 타이머 on
void DotweenTimer::Start()
{
	//Init();
}

void DotweenTimer::Update()
{
	if (isActive && m_localTimeScale > 0.001f)
	{
		elapsed += Time::GetDeltaTime() * m_localTimeScale;

		if (elapsed > duration)
			elapsed = duration;

		if (elapsed > delay)
		{
			onUpdate();
		}

		if (elapsed == duration)
		{
			if (isRepeat == false)
			{
				onCompleteFunction();
				onExpiration();
				isActive = false;
				elapsed = 0;
				isDone = true;
				//DotweenTimerPool::Instance().Return(this);
			}
			else
			{
				onCompleteFunction();
				elapsed = 0;
			}

		}
	}
}

