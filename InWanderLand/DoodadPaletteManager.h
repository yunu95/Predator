#pragma once
#ifdef EDITOR
#include "PaletteManager.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class DoodadPaletteManager : public PaletteManager, public yunutyEngine::SingletonClass<DoodadPaletteManager>
            {
            public:
                virtual void ApplyAsPlaytimeObjects() override {};
                virtual void CleanUpPlaytimeObjects() override {};
            private:
            };
        }
    }
}
#endif
