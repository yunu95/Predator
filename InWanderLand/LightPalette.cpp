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
        auto dl = TemplateDataManager::GetSingletonInstance().CreateTemplateData<Light_TemplateData>("Directional_Light");
        dl->pod.type = LightType::Directional;
        auto pl = TemplateDataManager::GetSingletonInstance().CreateTemplateData<Light_TemplateData>("Point_Light");
        pl->pod.type = LightType::Point;
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
        CleanUpData();
    }

    void LightPalette::CleanUpData()
    {
        Palette::CleanUpData();
        Reset();
    }
}
