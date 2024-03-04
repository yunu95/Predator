#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"
#include "InstanceManager.h"
#include "WaveData.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class WavePalette : public Palette, public  yunutyEngine::SingletonClass<WavePalette>
            {
            public:
                enum class WavePaletteState
                {
                    None,
                    SelectingTriggerRegion,
                    SelectingConstraintRegion,
                };
                WaveData* currentWaveData{ nullptr };
                WavePaletteState GetWavePaletteState() const { return wavePaletteState; }
                void SetWavePaletteState(WavePaletteState state) { wavePaletteState = state; }
            protected:
                virtual IEditableData* PlaceInstance(Vector3d worldPosition) override { return nullptr; }
                virtual bool ShouldSelect(IEditableData* instance)
                {
                    if (wavePaletteState == WavePaletteState::SelectingConstraintRegion || wavePaletteState == WavePaletteState::SelectingTriggerRegion)
                        return dynamic_cast<RegionData*>(instance);
                    return false;
                };
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
                virtual void OnSelectSingleInstance(IEditableData* data) override;
            private:
                WavePaletteState wavePaletteState{ WavePaletteState::None };
            };
        }
    }
}