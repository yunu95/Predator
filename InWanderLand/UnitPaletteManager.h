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
            class UnitPaletteManager : public PaletteManager, public yunutyEngine::SingletonClass<UnitPaletteManager>
            {
            public:
            private:
            };
        }
    }
}
