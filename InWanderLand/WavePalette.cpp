#include "WavePalette.h"
#include "WaveData.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
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
        }
    }
}