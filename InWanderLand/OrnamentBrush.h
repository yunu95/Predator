#pragma once
#include "YunutyEngine.h"
#include "SingletonComponent.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class OrnamentBrush : public yunutyEngine::Component, public yunutyEngine::SingletonComponent<OrnamentBrush>
            {
            public:
            private:
            };
        }
    }
}

