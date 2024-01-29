#include "InWanderLand.h"
#include "DebugMeshes.h"
#include "UnitInstance.h"
#include "UnitData.h"
namespace application::editor::palette
{
    void UnitInstance::Start()
    {
        PaletteInstance::Start();
        mesh = AttachDebugMesh(GetGameObject()->AddGameObject(), DebugMeshType::Cube, yunuGI::Color{1, 1, 1}, false);
    }
    void UnitInstance::Init(const application::editor::UnitData* unitData)
    {
        Init(unitData->pod.templateData);
    }
    void UnitInstance::Init(const application::editor::Unit_TemplateData* unitTemplateData)
    {
        this->unitTemplateData = unitTemplateData;
    }
}
