#include "LightPalette.h"
#include "WavePalette.h"
#include "LightEditorInstance.h"
#include "SelectionBox.h"
#include "InstanceManager.h"
#include "Light_TemplateData.h"
#include "LightData.h"
#include "TemplateDataManager.h"
#include "LightBrush.h"

namespace application::editor::palette
{
    LightData* LightPalette::GetSingleSelectedLight()
    {
        return selection.empty() ? nullptr : static_cast<LightData*>(const_cast<IEditableData*>(*selection.begin()));
    }

    void LightPalette::SelectLight(LightData* light)
    {
        Palette::OnSelectSingleInstance(light);
        LightBrush::Instance().ReadyBrush("");
    }

    void LightPalette::SelectLightTemplateData(Light_TemplateData* templateData)
    {
        selectedLightTemplateData = templateData;
        if (selectedLightTemplateData == nullptr)
        {
            LightBrush::Instance().ReadyBrush("");
        }
        else
        {
            LightBrush::Instance().ReadyBrush(selectedLightTemplateData->GetDataKey());
        }
    }

    void LightPalette::UnselectLightTemplateData()
    {
        SelectLightTemplateData(nullptr);
    }

    void LightPalette::Initialize()
    {
        auto pl = TemplateDataManager::GetSingletonInstance().CreateTemplateData<Light_TemplateData>("Point_Light");
        if (pl)
        {
            pl->pod.type = LightType::Point;
        }
    }

    void LightPalette::Reset()
    {
        Palette::Reset();
        UnselectLightTemplateData();
    }

    IEditableData* LightPalette::PlaceInstance(Vector3d worldPosition)
    {
        if (selectedLightTemplateData == nullptr)
            return nullptr;

        auto instance = InstanceManager::GetSingletonInstance().CreateInstance<LightData>(selectedLightTemplateData->GetDataKey());
        instance->pod.position.x = worldPosition.x;
        instance->pod.position.y = worldPosition.y;
        instance->pod.position.z = worldPosition.z;

        instance->ApplyAsPaletteInstance();
        return instance;
    }

    void LightPalette::OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos)
    {
        Palette::OnMouseMove(projectedWorldPos, normalizedScreenPos);
        // 브러시 움직이기
        LightBrush::Instance().GetTransform()->SetWorldPosition(projectedWorldPos);
        if (IsClickingLeft() && !IsSelectMode() && CheckInstantiationCooltime())
            PlaceInstance(projectedWorldPos);
    }

    void LightPalette::SetAsSelectMode(bool isSelectMode)
    {
        Palette::SetAsSelectMode(isSelectMode);
        if (isSelectMode)
        {
            LightBrush::Instance().ReadyBrush("");
        }
        else
        {
            SelectLightTemplateData(selectedLightTemplateData);
        }
    }

    bool LightPalette::ShouldSelect(IEditableData* instance)
    {
        return dynamic_cast<LightData*>(instance);
    }

    void LightPalette::OnStartPalette()
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
        LightBrush::Instance().GetGameObject()->SetSelfActive(true);
    }

    void LightPalette::OnStandbyPalette()
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
        LightBrush::Instance().GetGameObject()->SetSelfActive(false);
        CleanUpData();
    }
}
