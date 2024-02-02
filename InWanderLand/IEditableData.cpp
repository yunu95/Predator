#include "IEditableData.h"
#include "Terrain_TemplateData.h"
#include "Unit_TemplateData.h"
#include "Ornament_TemplateData.h"
#include "PaletteInstance.h"

namespace application::editor
{
    IEditableData::~IEditableData()
    {
        if (paletteInstance)
        {
            paletteInstance->editableData = nullptr;
            Scene::getCurrentScene()->DestroyGameObject(paletteInstance->GetGameObject());
        }
    }
    palette::PaletteInstance* IEditableData::GetPaletteInstance()
    {
        return paletteInstance;
    }
    void IEditableData::SetPaletteInstance(palette::PaletteInstance* paletteInstance)
    {
        this->paletteInstance = paletteInstance;
    }
    template<>
    DataType GetDataTypeEnum<Terrain_TemplateData>() { return DataType::TerrainData; }
    template<>
    DataType GetDataTypeEnum<Unit_TemplateData>() { return DataType::UnitData; }
    template<>
    DataType GetDataTypeEnum<Ornament_TemplateData>() { return DataType::OrnamentData; }
}
