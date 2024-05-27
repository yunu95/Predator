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
                OnSelectSingleInstance(region);
            }

            void RegionPalette::SetAsSelectingDisablingOrnaments(bool ornamentMode)
            {
                isSelectingDisablingOrnaments = ornamentMode;
                if (GetSingleSelectedRegion())
                {
                    for (auto each : GetSingleSelectedRegion()->GetDisablingOrnaments())
                    {
                        if (ornamentMode)
                        {
                            each->GetPaletteInstance()->OnSelected();
                        }
                        else
                        {
                            each->GetPaletteInstance()->OnDeselected();
                            each->GetPaletteInstance()->OnHover();
                        }
                    };
                }
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
                    auto selectedBefore = GetSingleSelectedRegion();
                    if (selectedBefore)
                    {
                        for (auto each : selectedBefore->GetDisablingOrnaments())
                        {
                            each->GetPaletteInstance()->OnDeselected();
                            each->GetPaletteInstance()->OnHoverLeft();
                        };
                    }
                    Palette::OnSelectEmpty();
                }
            }

            void RegionPalette::OnSelectSingleInstance(IEditableData* data)
            {
                Palette::OnSelectSingleInstance(data);
                if (dynamic_cast<RegionData*>(data))
                {
                    SetAsSelectingDisablingOrnaments(false);
                    auto selectedBefore = GetSingleSelectedRegion();
                    Palette::OnSelectSingleInstance(data);
                    if (selectedBefore)
                    {
                        for (auto each : selectedBefore->GetDisablingOrnaments())
                        {
                            each->GetPaletteInstance()->OnDeselected();
                            each->GetPaletteInstance()->OnHoverLeft();
                        };
                    }
                    if (GetSingleSelectedRegion())
                    {
                        for (auto each : GetSingleSelectedRegion()->GetDisablingOrnaments())
                        {
                            each->GetPaletteInstance()->OnHover();
                        };
                    }
                }
                else
                {
                    assert(GetSingleSelectedRegion());
                    OrnamentData* ornament{ dynamic_cast<OrnamentData*>(data) };
                    if (GetSingleSelectedRegion()->GetDisablingOrnaments().contains(ornament))
                    {
                        GetSingleSelectedRegion()->EraseDisablingOrnament(ornament);
                        ornament->GetPaletteInstance()->OnHoverLeft();
                        ornament->GetPaletteInstance()->OnDeselected();
                    }
                    else
                    {
                        GetSingleSelectedRegion()->AddDisablingOrnament(ornament);
                        ornament->GetPaletteInstance()->OnHover();
                        ornament->GetPaletteInstance()->OnSelected();
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

            void RegionPalette::CleanUpData()
            {
                SetAsSelectingDisablingOrnaments(false);
                OnSelectEmpty();
                Palette::CleanUpData();
            }
        }
    }
}