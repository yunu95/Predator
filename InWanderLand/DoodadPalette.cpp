#include "DoodadPalette.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            IEditableData* DoodadPalette::PlaceInstance(Vector3d worldPosition)
            {
                return nullptr;
            }

            bool DoodadPalette::ShouldSelect(IEditableData* instance)
            {
                return false;
            }

            void DoodadPalette::OnStartPalette()
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

            void DoodadPalette::OnStandbyPalette()
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
        }
    }
}
