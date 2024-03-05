#include "UnitPalette.h"
#include "WavePalette.h"
#include "UnitEditorInstance.h"
#include "SelectionBox.h"
#include "InstanceManager.h"
#include "Unit_TemplateData.h"
#include "UnitData.h"
#include "TemplateDataManager.h"
#include "UnitBrush.h"

namespace application::editor::palette
{
    void UnitPalette::SelectUnitTemplateData(Unit_TemplateData* templateData)
    {
        selectedUnitTemplateData = templateData;
        UnitBrush::Instance().ReadyBrush(selectedUnitTemplateData);
    }
    void UnitPalette::UnselectUnitTemplateData()
    {
        SelectUnitTemplateData(nullptr);
    }
    void UnitPalette::Reset()
    {
        UnselectUnitTemplateData();
    }
    UnitData* UnitPalette::PlaceInstance(Vector3d worldPosition)
    {
        auto instance = InstanceManager::GetSingletonInstance().CreateInstance<UnitData>(selectedUnitTemplateData->GetDataKey());
        instance->pod.position.x = worldPosition.x;
        instance->pod.position.y = worldPosition.y;
        instance->pod.position.z = worldPosition.z;

        auto wave = WavePalette::SingleInstance().currentWaveData;
        auto waveIdx = WavePalette::SingleInstance().currentSelectedWaveIndex;
        bool isWaveUnit = wave && waveIdx >= 0;
        if (isWaveUnit)
        {
            instance->pod.waveData = wave;
            wave->InsertUnitData({ .unitData = instance,.waveIdx = waveIdx,.delay = WavePalette::SingleInstance().currentSelectedWaveTimeOffset });
        }
        instance->ApplyAsPaletteInstance();
        return instance;
    }

    void UnitPalette::OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos)
    {
        Palette::OnMouseMove(projectedWorldPos, normalizedScreenPos);
        // 브러시 움직이기
        UnitBrush::Instance().GetTransform()->SetWorldPosition(projectedWorldPos);
        if (IsClickingLeft() && !IsSelectMode() && CheckInstantiationCooltime())
            PlaceInstance(projectedWorldPos);
    }

    void UnitPalette::SetAsSelectMode(bool isSelectMode)
    {
        Palette::SetAsSelectMode(isSelectMode);

        if (isSelectMode)
        {
            UnitBrush::Instance().ReadyBrush(nullptr);
        }
        else
        {
            UnitBrush::Instance().ReadyBrush(selectedUnitTemplateData);
        }
    }

    bool UnitPalette::ShouldSelect(IEditableData* instance)
    {
        return dynamic_cast<UnitData*>(instance);
    }

    void UnitPalette::OnStartPalette()
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

    void UnitPalette::OnStandbyPalette()
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
        WavePalette::SingleInstance().currentSelectedWaveIndex = -1;
    }

    void UnitPalette::CleanUpData()
    {
        Palette::CleanUpData();
        Reset();
    }
    void UnitPalette::Delete(IEditableData* data)
    {
        auto unitData = dynamic_cast<UnitData*>(data);
        if (unitData)
        {
            auto wave = unitData->pod.waveData;
            if (wave)
            {
                wave->DeleteUnitData(unitData);
            }
        }
        Palette::Delete(data);
    }
}
