#pragma once
#include "PaletteManager.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"

namespace Application
{
    namespace Editor
    {
        namespace Palette
        {
            class DoodadPaletteManager : public PaletteManager, public yunutyEngine::SingletonClass<DoodadPaletteManager>
            {
            public:
            private:
            };
        }
    }
}
