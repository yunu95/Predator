#pragma once
#include "YunutyYieldInstruction.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine
{
    namespace coroutine
    {
        class YUNUTY_API WaitForSeconds :public YieldInstruction
        {
        private:
            float elapsed{ 0 };
            float seconds{ 0 };
            bool isRealTime{ false };
        public:
            WaitForSeconds(float seconds, bool isRealTime = false);
            virtual void Update() override;
            virtual bool ShouldResume() const override;
            float GetExceededTime() const;
        };
    }
}
