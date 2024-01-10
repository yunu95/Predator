#pragma once
#include "PaletteManager.h"
#include "UnitInstance.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class UnitPaletteManager : public PaletteManager, public yunutyEngine::SingletonClass<UnitPaletteManager>
            {
            public:
            protected:
                virtual PaletteInstance* PlaceInstance(Vector3d worldPosition) override
                {
                    return PlaceSoleComponent<UnitInstance>(worldPosition);
                }
                virtual bool ShouldSelect(PaletteInstance* instance)
                {
                    return dynamic_cast<UnitInstance*>(instance);
                };
            private:
            };
        }
    }
}
