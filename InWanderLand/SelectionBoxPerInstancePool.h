#pragma once
#include "YunutyEngine.h"
#include "SelectionBoxPerInstance.h"
#include "SingletonComponent.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class SelectionBoxPerInstancePool :public GameObjectPool<SelectionBoxPerInstance>, public yunutyEngine::SingletonComponent<SelectionBoxPerInstancePool>
            {
            public:
            private:
            };
        }
    }
}
