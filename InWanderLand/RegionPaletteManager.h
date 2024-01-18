#pragma once
#ifdef EDITOR
#include "PaletteManager.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"
#include "RegionInstance.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class RegionPaletteManager : public PaletteManager, public  yunutyEngine::SingletonClass<RegionPaletteManager>
            {
            public:
            protected:
                virtual PaletteInstance* PlaceInstance(Vector3d worldPosition) override
                {
                    return PlaceSoleComponent<RegionInstance>(worldPosition);
                }
                virtual bool ShouldSelect(PaletteInstance* instance)
                {
                    return dynamic_cast<RegionInstance*>(instance);
                };
                virtual void ApplyAsPlaytimeObjects() override {};
                virtual void CleanUpPlaytimeObjects() override {};
            private:
            };
        }
    }
}
#endif
