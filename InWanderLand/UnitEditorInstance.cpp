#include "InWanderLand.h"
#include "DebugMeshes.h"
#include "UnitEditorInstance.h"
#include "UnitData.h"
namespace application::editor::palette
{
    void UnitEditorInstance::Start()
    {
        PaletteInstance::Start();
        mesh = AttachDebugMesh(GetGameObject()->AddGameObject(), DebugMeshType::Cube, yunuGI::Color{1, 1, 1}, false);
    }
    void UnitEditorInstance::Init(const application::editor::UnitData* unitData)
    {
        Init(unitData->pod.templateData);
    }
    void UnitEditorInstance::Init(const application::editor::Unit_TemplateData* unitTemplateData)
    {
        this->unitTemplateData = unitTemplateData;
    }
}
