#pragma once
#ifdef EDITOR
#include "YunutyEngine.h"
#include "SingletonComponent.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class UnitBrush : public yunutyEngine::Component, public yunutyEngine::SingletonComponent<UnitBrush>
            {
            public:
            private:
            };
        }
    }
}
#endif
