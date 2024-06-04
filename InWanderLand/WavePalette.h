#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"
#include "InstanceManager.h"
#include "WaveData.h"
#include "RegionData.h"

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
                void SetCurrentSelectedWaveTimeOffset(float timeOffset);
                float GetCurrentSelectedWaveTimeOffset() const { return currentSelectedWaveTimeOffset; }
                void UpdateWaveUnitsVisibility();
                void HideWaveUnitsVisibility();
                // 유닛 팔레트에서 유닛을 생성할 때 연계되는 웨이브의 인덱스,
                // currentWave가 null이 아니고 currentSelectedWaveIndex가 0보다 크거나 같으면 유닛 팔레트는 웨이브 유닛 생성 모드로 인식된다.
                int currentSelectedWaveIndex{ -1 };
                virtual void OnSelectSingleInstance(IEditableData* data) override;
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
            private:
                virtual void Reset() override;
                virtual void Delete(IEditableData* data)override;
                WavePaletteState wavePaletteState{ WavePaletteState::None };
                float currentSelectedWaveTimeOffset{ 0 };
                // 웨이브 유닛 배치정보를 수정할 때 생성시점이 오래된 유닛들은 숨김처리한다.
                // 이 변수는 그 숨김처리를 위한 시간을 저장한다. 생성되고 fadingTime초만큼 지나면 숨김처리된다.
                float waveIndicatorFadingTime{ 2.0f };
            };
        }
    }
}