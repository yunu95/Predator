#include "OrnamentPalette.h"
#include "OrnamentEditorInstance.h"
#include "SelectionBox.h"
#include "InstanceManager.h"
#include "Ornament_TemplateData.h"
#include "OrnamentData.h"
#include "TemplateDataManager.h"
#include "OrnamentBrush.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            void OrnamentPalette::SelectOrnamentTemplateData(Ornament_TemplateData* templateData)
            {
                selectedOrnamentTemplateData = templateData;
                OrnamentBrush::Instance().ReadyBrush(selectedOrnamentTemplateData);
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

            void OrnamentPalette::OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos)
            {
                Palette::OnMouseMove(projectedWorldPos, normalizedScreenPos);
                // 브러시 움직이기
                OrnamentBrush::Instance().GetTransform()->SetWorldPosition(projectedWorldPos);
                if (IsClickingLeft() && !IsSelectMode())
                {
                    if (CheckInstantiationCooltime())
                        PlaceInstance(projectedWorldPos);
                }
            }

            void OrnamentPalette::SetAsSelectMode(bool isSelectMode)
            {
                Palette::SetAsSelectMode(isSelectMode);

                if (isSelectMode)
                {
                    OrnamentBrush::Instance().ReadyBrush(nullptr);
                }
                else
                {
                    OrnamentBrush::Instance().ReadyBrush(selectedOrnamentTemplateData);
                }
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
