#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"
#include "RegionEditorInstance.h"
#include "InstanceManager.h"
#include "OrnamentData.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class RegionPalette : public Palette, public yunutyEngine::SingletonClass<RegionPalette>
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
                void SetAsSelectingDisablingOrnaments(bool ornamentMode);
                bool GetIsSelectingDisablingOrnaments();
            protected:
                virtual IEditableData* PlaceInstance(Vector3d worldPosition) override;
                virtual bool ShouldSelect(IEditableData* instance) override;
                virtual void OnSelectEmpty()override;
                virtual void OnSelectSingleInstance(IEditableData* data) override;
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
            private:
                bool isSelectingDisablingOrnaments{false};
            };
        }
    }
}
