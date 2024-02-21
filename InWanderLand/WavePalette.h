#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"
#include "InstanceManager.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class WavePalette : public Palette, public  yunutyEngine::SingletonClass<WavePalette>
            {
            public:
            protected:
                virtual IEditableData* PlaceInstance(Vector3d worldPosition) override { return nullptr; }
                virtual bool ShouldSelect(IEditableData* instance) { return false; };
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
            private:
            };
        }
    }
}