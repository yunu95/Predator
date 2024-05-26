#pragma once
#include "YunutyTime.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine
{
    namespace coroutine
    {
        class YUNUTY_API ForSeconds
        {
        private:
            float elapsed = 0;
            float duration = 1;
        public:
            ForSeconds(float duration, bool isRealTime = false);
            ForSeconds(ForSeconds&& other) = delete;
            ForSeconds(ForSeconds& other) = delete;
            bool isRealTime = false;
            // elapsed에 deltaTime을 더하고, 경과시간이 duration보다 작으면 true를 반환합니다.
            // deltaTime을 더하기 이전에 이미 경과시간이 duration과 같다면 false를 반환합니다.
            // 경과시간은 duration보다 클 수 없습니다.
            bool Tick();
            // 경과시간을 반환합니다.
            float Elapsed();
            // 경과시간을 0~1 사이의 값으로 반환합니다.
            float ElapsedNormalized();
        };
    }
}
