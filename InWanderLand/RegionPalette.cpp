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

            void RegionPalette::SetAsSelectingDisablingOrnaments(bool ornamentMode)
            {
                isSelectingDisablingOrnaments = ornamentMode;
            }

            bool RegionPalette::GetIsSelectingDisablingOrnaments()
            {
                return isSelectingDisablingOrnaments;
            }

            IEditableData* RegionPalette::PlaceInstance(Vector3d worldPosition)
            {
                //InstanceManager::GetSingletonInstance().CreateInstance<RegionData>();
                return nullptr;
            }

            bool RegionPalette::ShouldSelect(IEditableData* instance)
            {
                if (isSelectingDisablingOrnaments && GetSingleSelectedRegion())
                {
                    return dynamic_cast<OrnamentData*>(instance);
                }
                return dynamic_cast<RegionData*>(instance);
            }

            void RegionPalette::OnSelectEmpty()
            {
                if (!isSelectingDisablingOrnaments)
                {
                    Palette::OnSelectEmpty();
                }
            }

            void RegionPalette::OnSelectSingleInstance(IEditableData* data)
            {
                if (dynamic_cast<RegionData*>(data))
                {
                    Palette::OnSelectSingleInstance(data);
                }
                else
                {
                    assert(GetSingleSelectedRegion());
                    OrnamentData* ornament{ dynamic_cast<OrnamentData*>(data) };
                    if (GetSingleSelectedRegion()->GetDisablingOrnaments().contains(ornament))
                    {
                        GetSingleSelectedRegion()->EraseDisablingOrnament(ornament);
                        ornament->GetPaletteInstance()->OnSelected();
                    }
                    else
                    {
                        GetSingleSelectedRegion()->AddDisablingOrnament(ornament);
                        ornament->GetPaletteInstance()->OnDeselected();
                    }
                }
            }

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