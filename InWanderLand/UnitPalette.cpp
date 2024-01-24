#include "UnitPalette.h"
#include "UnitInstance.h"
#include "SelectionBox.h"

namespace application::editor::palette
{
    PaletteInstance* UnitPalette::PlaceInstance(Vector3d worldPosition)
    {
        return PlaceSoleComponent<UnitInstance>(worldPosition);
    }

    bool UnitPalette::ShouldSelect(PaletteInstance* instance)
    {
        return dynamic_cast<UnitInstance*>(instance);
    }

    void UnitPalette::OnStartPalette()
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

    void UnitPalette::OnStandbyPalette()
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
