#pragma once
#include "YunutyEngine.h"
#include "SelectionBoxPerInstance.h"
#include "SingletonComponent.h"

namespace Application
{
    namespace Editor
    {
        namespace Palette
        {
            class SelectionBoxPerInstancePool :public GameObjectPool<SelectionBoxPerInstance>, public yunutyEngine::SingletonComponent<SelectionBoxPerInstancePool>
            {
            public:
            private:
            };
        }
    }
}
