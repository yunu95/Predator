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
            class UnitPaletteManager : public PaletteManager, public yunutyEngine::SingletonClass<UnitPaletteManager>
            {
            public:
            protected:
                virtual PaletteInstance* PlaceInstance(Vector3d worldPosition) override;
                virtual bool ShouldSelect(PaletteInstance* instance) override;
                virtual void ApplyAsPlaytimeObjects() override {};
                virtual void CleanUpPlaytimeObjects() override {};
            private:
            };
        }
    }
}
#endif
