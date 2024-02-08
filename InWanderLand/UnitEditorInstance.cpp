#include "InWanderLand.h"
#include "DebugMeshes.h"
#include "UnitEditorInstance.h"
#include "UnitData.h"
namespace application::editor::palette
{
    void UnitEditorInstance::Start()
    {
        PaletteInstance::Start();
    }
    void UnitEditorInstance::Init(const application::editor::UnitData* unitData)
    {
        Init(unitData->pod.templateData);
    }
    void UnitEditorInstance::Init(const application::editor::Unit_TemplateData* unitTemplateData)
    {
        this->unitTemplateData = unitTemplateData;
        auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(unitTemplateData->pod.fbxName);
        obj->setName("FBX");
        obj->SetParent(GetGameObject());
    }

    void UnitEditorInstance::ChangeTemplateData(const application::editor::UnitData* unitData)
    {
        ChangeTemplateData(unitData->pod.templateData);
    }

    void UnitEditorInstance::ChangeTemplateData(const application::editor::Unit_TemplateData* unitTemplateData)
    {
        if (this->unitTemplateData == unitTemplateData)
            return;

        this->unitTemplateData = unitTemplateData;
        for (auto& each : GetGameObject()->GetChildren())
        {
            if (each->getName() == "FBX")
            {
                yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(each);
                break;
            }
        }
        auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(unitTemplateData->pod.fbxName);
        obj->setName("FBX");
        obj->SetParent(GetGameObject());
    }
}
