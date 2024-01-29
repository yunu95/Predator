#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"
#include "RegionInstance.h"
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
            protected:
                virtual IEditableData* PlaceInstance(Vector3d worldPosition) override;
                virtual bool ShouldSelect(IEditableData* instance)
                {
                    return dynamic_cast<RegionInstance*>(instance);
                };
                //virtual void ApplyAsPlaytimeObjects() override {};
                //virtual void CleanUpPlaytimeObjects() override {};
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
            private:
            };
        }
    }
}
