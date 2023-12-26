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
            class TerrainPaletteManager : public PaletteManager, public  yunutyEngine::SingletonClass<TerrainPaletteManager>
            {
            public:
            private:
            };
        }
    }
}
