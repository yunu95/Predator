#include "Timer.h"
#include "YunutyTime.h"
using namespace yunutyEngine;

WanderUtil::Timer::Timer()
	: isActive(false), isRepeat(false), elapsed(0.0f), duration(1.0f)
{

}

void WanderUtil::Timer::Start()
{
	isActive = true;
	elapsed = 0.0f;
}

void WanderUtil::Timer::Update()
{
	if (isActive)
	{
		elapsed += Time::GetDeltaTime();

		if (elapsed > duration)
			elapsed = duration;

		if (onTimerElapsed)
		{
			onTimerElapsed(elapsed / duration);
		}

		if (elapsed == duration)
		{
			if (isRepeat)
			{
				elapsed = 0.0f;
			}
			else
			{
				isActive = false;
			}
			onTimerExpiration();
		}
	}
}
