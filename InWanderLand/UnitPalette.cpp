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
        if (selectedUnitTemplateData == nullptr)
        {
            UnitBrush::Instance().ReadyBrush("");
        }
        else
        {
            UnitBrush::Instance().ReadyBrush(selectedUnitTemplateData->GetDataKey());
        }
    }

    void UnitPalette::UnselectUnitTemplateData()
    {
        SelectUnitTemplateData(nullptr);
    }

    UnitData* UnitPalette::GetSingleSelectedUnitInstance()
    {
        return (selection.size() != 1) ? nullptr : static_cast<UnitData*>(const_cast<IEditableData*>(*selection.begin()));
    }

    void UnitPalette::SelectUnitInstance(UnitData* unit)
    {
        Palette::OnSelectSingleInstance(unit);
    }

    void UnitPalette::Reset()
    {
        Palette::Reset();
        UnselectUnitTemplateData();
    }

    IEditableData* UnitPalette::PlaceInstance(Vector3d worldPosition)
    {
        if (selectedUnitTemplateData == nullptr)
            return nullptr;

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
            wave->InsertUnitData({ .unitData = instance,.waveIdx = waveIdx,.delay = WavePalette::SingleInstance().GetCurrentSelectedWaveTimeOffset() });
            wave->ApplyMapAsPod();
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
            UnitBrush::Instance().ReadyBrush("");
        }
        else
        {
            SelectUnitTemplateData(selectedUnitTemplateData);
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
        UnitBrush::Instance().GetGameObject()->SetSelfActive(true);
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
        UnitBrush::Instance().GetGameObject()->SetSelfActive(false);
        CleanUpData();
        if (WavePalette::SingleInstance().currentSelectedWaveIndex >= 0)
        {
            WavePalette::SingleInstance().HideWaveUnitsVisibility();
            WavePalette::SingleInstance().currentSelectedWaveIndex = -1;
        }
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
