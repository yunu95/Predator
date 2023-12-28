#pragma once
#include "PaletteManager.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class TerrainPaletteManager : public PaletteManager, public  yunutyEngine::SingletonClass<TerrainPaletteManager>
            {
            public:
                virtual PaletteInstance* PlaceInstance(Vector3d worldPosition) { return nullptr; };
                virtual bool isSelectMode() { return false; };
            private:
            };
        }
    }
}
