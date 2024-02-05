#include "OrnamentPalette.h"
#include "OrnamentEditorInstance.h"
#include "SelectionBox.h"
#include "InstanceManager.h"
#include "Ornament_TemplateData.h"
#include "OrnamentData.h"
#include "TemplateDataManager.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            void OrnamentPalette::SelectOrnamentTemplateData(Ornament_TemplateData* templateData)
            {
                selectedOrnamentTemplateData = templateData;
            }

            void OrnamentPalette::UnselectOrnamentTemplateData()
            {
                SelectOrnamentTemplateData(nullptr);
            }

            void OrnamentPalette::Reset()
            {
                UnselectOrnamentTemplateData();
            }

            IEditableData* OrnamentPalette::PlaceInstance(Vector3d worldPosition)
            {
                auto instance = InstanceManager::GetSingletonInstance().CreateInstance<OrnamentData>(selectedOrnamentTemplateData->GetDataKey());
                instance->pod.position.x = worldPosition.x;
                instance->pod.position.y = worldPosition.y;
                instance->pod.position.z = worldPosition.z;

                instance->ApplyAsPaletteInstance();
                return instance;
            }

            bool OrnamentPalette::ShouldSelect(IEditableData* instance)
            {
                return dynamic_cast<OrnamentData*>(instance);
            }

            void OrnamentPalette::OnStartPalette()
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

            void OrnamentPalette::OnStandbyPalette()
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

            void OrnamentPalette::CleanUpData()
            {
                Palette::CleanUpData();
                Reset();
            }
        }
    }
}
