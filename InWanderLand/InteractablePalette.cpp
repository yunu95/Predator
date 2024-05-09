#include "InteractablePalette.h"
#include "WavePalette.h"
#include "InteractableEditorInstance.h"
#include "SelectionBox.h"
#include "InstanceManager.h"
#include "Interactable_TemplateData.h"
#include "InteractableData.h"
#include "TemplateDataManager.h"
#include "InteractableBrush.h"

namespace application::editor::palette
{
    void InteractablePalette::SelectInteractableTemplateData(Interactable_TemplateData* templateData)
    {
        selectedInteractableTemplateData = templateData;
        if (selectedInteractableTemplateData == nullptr)
        {
            InteractableBrush::Instance().ReadyBrush("");
        }
        else
        {
            InteractableBrush::Instance().ReadyBrush(selectedInteractableTemplateData->GetDataKey());
        }
    }

    void InteractablePalette::UnselectInteractableTemplateData()
    {
        SelectInteractableTemplateData(nullptr);
    }

    void InteractablePalette::Reset()
    {
        Palette::Reset();
        UnselectInteractableTemplateData();
    }

    IEditableData* InteractablePalette::PlaceInstance(Vector3d worldPosition)
    {
        if (selectedInteractableTemplateData == nullptr)
            return nullptr;

        auto instance = InstanceManager::GetSingletonInstance().CreateInstance<InteractableData>(selectedInteractableTemplateData->GetDataKey());
        instance->pod.position.x = worldPosition.x;
        instance->pod.position.y = worldPosition.y;
        instance->pod.position.z = worldPosition.z;
        instance->ApplyAsPaletteInstance();
        return instance;
    }

    void InteractablePalette::OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos)
    {
        Palette::OnMouseMove(projectedWorldPos, normalizedScreenPos);
        // 브러시 움직이기
        InteractableBrush::Instance().GetTransform()->SetWorldPosition(projectedWorldPos);
        if (IsClickingLeft() && !IsSelectMode() && CheckInstantiationCooltime())
            PlaceInstance(projectedWorldPos);
    }

    void InteractablePalette::SetAsSelectMode(bool isSelectMode)
    {
        Palette::SetAsSelectMode(isSelectMode);

        if (isSelectMode)
        {
            InteractableBrush::Instance().ReadyBrush("");
        }
        else
        {
            SelectInteractableTemplateData(selectedInteractableTemplateData);
        }
    }

    bool InteractablePalette::ShouldSelect(IEditableData* instance)
    {
        return dynamic_cast<InteractableData*>(instance);
    }

    void InteractablePalette::OnStartPalette()
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
        InteractableBrush::Instance().GetGameObject()->SetSelfActive(true);
    }

    void InteractablePalette::OnStandbyPalette()
    {
        if (IsSelectMode())
        {
            beforeState = State::Select;
        }
        else
        {
            beforeState = State::Place;
        }
        InteractableBrush::Instance().GetGameObject()->SetSelfActive(false);
        CleanUpData();
    }
}
