#include "UnitPaletteManager.h"
#include "UnitInstance.h"

namespace application::editor::palette
{
    PaletteInstance* UnitPaletteManager::PlaceInstance(Vector3d worldPosition)
    {
        return PlaceSoleComponent<UnitInstance>(worldPosition);
    };
    bool UnitPaletteManager::ShouldSelect(PaletteInstance* instance)
    {
        return dynamic_cast<UnitInstance*>(instance);
    };
}
