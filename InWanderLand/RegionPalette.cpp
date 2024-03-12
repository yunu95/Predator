#include "RegionPalette.h"
#include "RegionData.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            RegionData* RegionPalette::GetSingleSelectedRegion()
            {
                return selection.empty() ? nullptr : static_cast<RegionData*>(const_cast<IEditableData*>(*selection.begin()));
            }

            void RegionPalette::SelectRegion(RegionData* region)
            {
                Palette::OnSelectSingleInstance(region);
            }

            IEditableData* RegionPalette::PlaceInstance(Vector3d worldPosition)
            {
                //InstanceManager::GetSingletonInstance().CreateInstance<RegionData>();
                return nullptr;
            };
            void RegionPalette::OnStartPalette()
            {
                switch (beforeState)
                {
                case application::editor::palette::Palette::State::Place:
                {
                    SetAsSelectMode(false);
                    break;
                }
                default:
                    SetAsSelectMode(true);
                    break;
                }
            }

            void RegionPalette::OnStandbyPalette()
            {
                if (IsSelectMode())
                {
                    beforeState = State::Select;
                }
                else
                {
                    beforeState = State::Place;
                }
                state = State::None;
                CleanUpData();
            }

            bool RegionPalette::ShouldSelect(IEditableData* instance)
            {
                return dynamic_cast<RegionData*>(instance);
            }
        }
    }
}