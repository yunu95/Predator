#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"

namespace application
{
    namespace editor
    {
		class UnitData;
        namespace palette
        {
            class DoodadPalette : public Palette, public yunutyEngine::SingletonClass<DoodadPalette>
            {
            public:
                virtual IEditableData* PlaceInstance(Vector3d worldPosition) override;
                virtual bool ShouldSelect(IEditableData* instance) override;
                //virtual void ApplyAsPlaytimeObjects() override {};
                //virtual void CleanUpPlaytimeObjects() override {};
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
            private:
            };
        }
    }
}
