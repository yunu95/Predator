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
                // 유닛 팔레트에서 유닛을 생성할 때 연계되는 웨이브의 인덱스,
                // currentWave가 null이 아니고 currentSelectedWaveIndex가 0보다 크거나 같으면 유닛 팔레트는 웨이브 유닛 생성 모드로 인식된다.
                int currentSelectedWaveIndex{ -1 };
                float currentSelectedWaveTimeOffset{ 0 };
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