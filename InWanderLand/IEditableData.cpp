#include "IEditableData.h"
#include "Terrain_TemplateData.h"
#include "Unit_TemplateData.h"
#include "Ornament_TemplateData.h"

namespace application::editor
{
    template<>
    DataType GetDataTypeEnum<Terrain_TemplateData>() { return DataType::TerrainData; }
    template<>
    DataType GetDataTypeEnum<Unit_TemplateData>() { return DataType::UnitData; }
    template<>
    DataType GetDataTypeEnum<Ornament_TemplateData>() { return DataType::OrnamentData; }
}
