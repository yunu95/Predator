#include "TimePauseTimer.h"

void TimePauseTimer::Init()
{
    isRealtime = true;
    onUpdate = [this](float t)
        {
            Time::SetTimeScale(math::LerpF(1, pausedTimeScale, sinf(t * 0.5 * math::PI)));
        };
}
