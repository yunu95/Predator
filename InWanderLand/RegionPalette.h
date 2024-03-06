#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"
#include "RegionEditorInstance.h"
#include "InstanceManager.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class RegionPalette : public Palette, public  yunutyEngine::SingletonClass<RegionPalette>
            {
            public:
                RegionData* GetSingleSelectedRegion()
                {
                    return selection.empty() ? nullptr : static_cast<RegionData*>(*selection.begin());
                }
                void SelectRegion(RegionData* region)
                {
                    Palette::OnSelectSingleInstance(region);
                }
            protected:
                virtual IEditableData* PlaceInstance(Vector3d worldPosition) override;
                virtual bool ShouldSelect(IEditableData* instance)
                {
                    return dynamic_cast<RegionData*>(instance);
                };
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
            private:
            };
        }
    }
}
