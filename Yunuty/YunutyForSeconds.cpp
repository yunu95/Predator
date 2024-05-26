#include "YunutyForSeconds.h"
#include "YunutyTime.h"

namespace coroutine
{
    ForSeconds::ForSeconds(float duration, bool isRealTime = false) : duration(duration), isRealTime(isRealTime) {}
    bool ForSeconds::Tick()
    {
        if (elapsed == duration)
            return false;
        elapsed += isRealTime ? yunutyEngine::Time::GetDeltaTimeUnscaled() : yunutyEngine::Time::GetDeltaTime();
        elapsed = std::min(elapsed, duration);
        return true;
    }
    float ForSeconds::Elapsed()
    {
        return elapsed;
    }
    float ForSeconds::ElapsedNormalized()
    {
        return elapsed / duration;
    }
}
