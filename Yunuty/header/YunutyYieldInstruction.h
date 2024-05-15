#pragma once

namespace yunutyEngine
{
    namespace coroutine
    {
        class YieldInstruction
        {
        public:
            virtual bool ShouldResume() const = 0;
            virtual void Update() = 0;
        };
    }
}
