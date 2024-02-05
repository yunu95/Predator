#include "UnitPalette.h"
#include "UnitEditorInstance.h"
#include "SelectionBox.h"
#include "InstanceManager.h"
#include "Unit_TemplateData.h"
#include "UnitData.h"
#include "TemplateDataManager.h"


namespace application::editor::palette
{
    void UnitPalette::SelectUnitTemplateData(Unit_TemplateData* templateData)
    {
        selectedUnitTemplateData = templateData;
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

        instance->ApplyAsPaletteInstance();
        return instance;
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
    }

    void UnitPalette::CleanUpData()
    {
        Palette::CleanUpData();
        Reset();
    }
}
