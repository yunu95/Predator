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
        yunuGI::Vector3 boundingMin, boundingMax;
        auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(unitTemplateData->pod.skinnedFBXName, &boundingMin, &boundingMax);
        obj->SetParent(GetGameObject());
        currentFBX = unitTemplateData->pod.skinnedFBXName;
    }

    void UnitEditorInstance::ChangeTemplateData(const application::editor::UnitData* unitData)
    {
        ChangeTemplateData(unitData->pod.templateData);
    }

    void UnitEditorInstance::ChangeTemplateData(const application::editor::Unit_TemplateData* unitTemplateData)
    {
        if (unitTemplateData == nullptr)
            return;
        this->unitTemplateData = unitTemplateData;
        ChangeResource(unitTemplateData->pod.skinnedFBXName);
    }

    void UnitEditorInstance::ChangeResource(const std::string& fbxName)
    {
        // TemplateData 를 유지하고 Resource 만 갱신함
        if (currentFBX == fbxName)
            return;

        for (auto& each : GetGameObject()->GetChildren())
        {
            if (each->getName() == currentFBX)
            {
                yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(each);
                break;
            }
        }

        auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(fbxName);
        obj->SetParent(GetGameObject());
        currentFBX = fbxName;

        return;
    }
}
