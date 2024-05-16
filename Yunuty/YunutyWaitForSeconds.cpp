#include "YunutyEngine.h"
#include "YunutyWaitForSeconds.h"

namespace yunutyEngine
{
    namespace coroutine
    {
        WaitForSeconds::WaitForSeconds(float seconds, bool isRealTime)
        {
            this->seconds = seconds;
            this->isRealTime = isRealTime;
        }
        void WaitForSeconds::Update()
        {
            elapsed += isRealTime ? Time::GetDeltaTimeUnscaled() : Time::GetDeltaTime();
        }
        bool WaitForSeconds::ShouldResume() const
        {
            return elapsed >= seconds;
        }
        float WaitForSeconds::GetExceededTime() const
        {
            return elapsed - seconds;
        }
    }
}