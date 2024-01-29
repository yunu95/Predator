#include "UnitPalette.h"
#include "UnitInstance.h"
#include "SelectionBox.h"
#include "InstanceManager.h"
#include "Unit_TemplateData.h"
#include "UnitData.h"


namespace application::editor::palette
{
    unordered_set<UnitInstance*>& UnitPalette::GetAllInstances()
    {
        return unitInstances;
    }
    void UnitPalette::ClearAllInstances()
    {
    }
    void UnitPalette::AddUnitInstance(const UnitData& unitData)
    {

    }
    void UnitPalette::SelectUnitTemplateData(Unit_TemplateData* templateData)
    {
        selectedUnitTemplateData = templateData;
    }
    void UnitPalette::UnselectUnitTemplateData()
    {
        SelectUnitTemplateData(nullptr);
    }
    UnitData* UnitPalette::PlaceInstance(Vector3d worldPosition)
    {
        auto instance = InstanceManager::GetSingletonInstance().CreateInstance<UnitData>(selectedUnitTemplateData->GetDataKey());
        instance->pod.x = worldPosition.x;
        instance->pod.y = worldPosition.y;
        instance->pod.z = worldPosition.z;

        instance->ApplyAsPaletteInstance();
        return instance;
    }

    bool UnitPalette::ShouldSelect(IEditableData* instance)
    {
        return dynamic_cast<UnitInstance*>(instance);
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
    }
}
