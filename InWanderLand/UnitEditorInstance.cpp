#include "InWanderLand.h"
#include "DebugMeshes.h"
#include "UnitEditorInstance.h"
#include "UnitData.h"

#include "EditorResourceManager.h"

namespace application::editor::palette
{
    void UnitEditorInstance::Start()
    {
        PaletteInstance::Start();
    }

    void UnitEditorInstance::Init(const application::editor::UnitData* unitData)
    {
        Init(unitData->pod.templateData);

        if (unitData->pod.isGuide)
        {
            ChangeGuideInstance();
        }
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
        static ResourceManager& erm = ResourceManager::GetSingletonInstance();

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

        if (isGuide)
        {
            auto comp = obj->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

            if (comp)
            {
                for (int i = 0; i < comp->GetGI().GetMaterialCount(); ++i)
                {
                    comp->GetGI().GetMaterial(i)->SetPixelShader(erm.GetShader("Debug_AlphaPS.cso"));
                    comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,0,0,0.2 });
                }
            }
        }

        obj->SetParent(GetGameObject());
        currentFBX = fbxName;

        return;
    }

    void UnitEditorInstance::ChangeGuideInstance()
    {
        static ResourceManager& erm = ResourceManager::GetSingletonInstance();

        for (auto each : GetGameObject()->GetChildren())
        {
            if (each->getName() != currentFBX)
            {
                continue;
            }

            auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

            if (comp)
            {
                for (int i = 0; i < comp->GetGI().GetMaterialCount(); ++i)
                {
                    comp->GetGI().GetMaterial(i)->SetPixelShader(erm.GetShader("Debug_AlphaPS.cso"));
                    comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,1,1,0.2 });
                }
            }
        }

        GetGameObject()->SetSelfActive(false);

        isGuide = true;
    }

    void UnitEditorInstance::ShowEditorInstance()
    {
        if (isGuide)
        {
            return;
        }

        GetGameObject()->SetSelfActive(true);
    }
}
