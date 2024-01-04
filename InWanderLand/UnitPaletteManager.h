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
            class UnitPaletteManager : public PaletteManager, public yunutyEngine::SingletonClass<UnitPaletteManager>
            {
            public:
            private:
            };
        }
    }
}
