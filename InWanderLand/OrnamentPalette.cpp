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
                if (selectedOrnamentTemplateData == nullptr)
                {
                    OrnamentBrush::Instance().ReadyBrush("");
                }
                else
                {
                    OrnamentBrush::Instance().ReadyBrush(selectedOrnamentTemplateData->GetDataKey());
                }
            }

            void OrnamentPalette::UnselectOrnamentTemplateData()
            {
                SelectOrnamentTemplateData(nullptr);
            }

            OrnamentData* OrnamentPalette::GetSingleSelectedOrnamentInstance()
            {
                return (selection.size() != 1) ? nullptr : static_cast<OrnamentData*>(const_cast<IEditableData*>(*selection.begin()));
            }

            void OrnamentPalette::SelectOrnamentInstance(OrnamentData* ornament)
            {
                Palette::OnSelectSingleInstance(ornament);
            }

            void OrnamentPalette::Reset()
            {
                Palette::Reset();
                UnselectOrnamentTemplateData();
            }

            IEditableData* OrnamentPalette::PlaceInstance(Vector3d worldPosition)
            {
                if (selectedOrnamentTemplateData == nullptr)
                    return nullptr;

                auto instance = InstanceManager::GetSingletonInstance().CreateInstance<OrnamentData>(selectedOrnamentTemplateData->GetDataKey());
                instance->pod.position.x = worldPosition.x;
                instance->pod.position.y = worldPosition.y;
                instance->pod.position.z = worldPosition.z;
                instance->pod.madeEditor = true;

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
                    OrnamentBrush::Instance().ReadyBrush("");
                }
                else
                {
                    SelectOrnamentTemplateData(selectedOrnamentTemplateData);
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
                OrnamentBrush::Instance().GetGameObject()->SetSelfActive(true);
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
                OrnamentBrush::Instance().GetGameObject()->SetSelfActive(false);
                CleanUpData();
            }
        }
    }
}
