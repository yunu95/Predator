#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"

namespace application
{
    namespace editor
    {
        class RegionData;
    }
}

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class RegionPalette : public Palette, public yunutyEngine::SingletonClass<RegionPalette>
            {
            public:
                RegionData* GetSingleSelectedRegion();
                void SelectRegion(RegionData* region);
            protected:
                virtual IEditableData* PlaceInstance(Vector3d worldPosition) override;
                virtual bool ShouldSelect(IEditableData* instance) override;
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
            private:
            };
        }
    }
}
