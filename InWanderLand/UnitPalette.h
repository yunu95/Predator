#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class UnitPalette : public Palette, public yunutyEngine::SingletonClass<UnitPalette>
            {
            public:
            protected:
                virtual PaletteInstance* PlaceInstance(Vector3d worldPosition) override;
                virtual bool ShouldSelect(PaletteInstance* instance) override;
                virtual void ApplyAsPlaytimeObjects() override {};
                virtual void CleanUpPlaytimeObjects() override {};
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
            private:
            };
        }
    }
}
