#include "WavePalette.h"
#include "WaveData.h"
#include "UnitData.h"
#include "RegionData.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            void WavePalette::SetCurrentSelectedWaveTimeOffset(float timeOffset)
            {
                currentSelectedWaveTimeOffset = timeOffset;
                UpdateWaveUnitsVisibility();
            }
            void WavePalette::UpdateWaveUnitsVisibility()
            {
                if (currentWaveData && currentSelectedWaveIndex >= 0)
                {
                    // 현재 시점으로부터 fading time초 전에 생성된 유닛들은 숨김처리한다.
                    // 현재 아직 생성되지 않은 유닛들도 숨김처리한다.
                    for (auto each : currentWaveData->GetWaveUnitDataMap())
                    {
                        bool shouldVisible = each.second.waveIdx == currentSelectedWaveIndex && currentSelectedWaveTimeOffset - waveIndicatorFadingTime < each.second.delay && each.second.delay <= currentSelectedWaveTimeOffset;
                        each.first->GetPaletteInstance()->GetGameObject()->SetSelfActive(shouldVisible);
                    }
                }
            }
            void WavePalette::HideWaveUnitsVisibility()
            {
                if (currentWaveData && currentSelectedWaveIndex >= 0)
                {
                    // 유닛들을 모두 숨김처리한다.
                    for (auto each : currentWaveData->GetWaveUnitDataMap())
                    {
                        each.first->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
                    }
                }
            }
            void WavePalette::OnStartPalette()
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

            void WavePalette::OnStandbyPalette()
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
            void WavePalette::OnSelectSingleInstance(IEditableData* data)
            {
                switch (wavePaletteState)
                {
                case WavePaletteState::None:
                    break;
                case WavePaletteState::SelectingConstraintRegion:
                    currentWaveData->pod.contraintRegion = dynamic_cast<RegionData*>(data);
                    wavePaletteState = WavePaletteState::None;
                    break;
                case WavePaletteState::SelectingTriggerRegion:
                    currentWaveData->pod.triggerRegion = dynamic_cast<RegionData*>(data);
                    wavePaletteState = WavePaletteState::None;
                    break;
                }
            }
            void WavePalette::Reset() 
            {
                Palette::Reset();
                currentSelectedWaveIndex = -1;
                currentSelectedWaveTimeOffset = 0;
                currentWaveData = nullptr;
            }
        }
    }
}