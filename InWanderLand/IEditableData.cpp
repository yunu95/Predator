#include "IEditableData.h"
#include "TemplateDataList.h"
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
    template<>
    DataType GetDataTypeEnum<Region_TemplateData>() { return DataType::RegionData; }
    template<>
    DataType GetDataTypeEnum<Wave_TemplateData>() { return DataType::WaveData; }
    template<>
    DataType GetDataTypeEnum<Camera_TemplateData>() { return DataType::CameraData; }
    template<>
    DataType GetDataTypeEnum<Light_TemplateData>() { return DataType::LightData; }
    template<>
    DataType GetDataTypeEnum<Particle_TemplateData>() { return DataType::ParticleData; }
    template<>
    DataType GetDataTypeEnum<Interactable_TemplateData>() { return DataType::InteractableData; }
}
