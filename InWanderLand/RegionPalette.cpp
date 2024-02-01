#include "RegionPalette.h"
#include "RegionData.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
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
        }
    }
}