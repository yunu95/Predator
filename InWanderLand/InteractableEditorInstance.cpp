#include "InWanderLand.h"
#include "DebugMeshes.h"
#include "InteractableEditorInstance.h"
#include "InteractableData.h"
#include "EditorResourceManager.h"
#include "DebugMeshes.h"

namespace application::editor::palette
{
    void InteractableEditorInstance::Start()
    {
        PaletteInstance::Start();
    }

    void InteractableEditorInstance::Init(const application::editor::InteractableData* interactableData)
    {
        Init(interactableData->pod.templateData);
    }

    void InteractableEditorInstance::Init(const application::editor::Interactable_TemplateData* interactableTemplateData)
    {
        this->interactableTemplateData = interactableTemplateData;
        yunuGI::Vector3 boundingMin, boundingMax;

        GameObject* obj = nullptr;

        if (interactableTemplateData->pod.fBXName == "Trigger_Cube")
        {
            obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Cube", &boundingMin, &boundingMax);
            for (auto each : obj->GetChildren())
            {
                auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
                if (comp)
                {
                    yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(each);
                    break;
                }
            }
            AttachDebugMesh(obj, DebugMeshType::Cube, yunuGI::Color::green());
        }
        else if (interactableTemplateData->pod.fBXName == "Trigger_Sphere")
        {
            obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Sphere", &boundingMin, &boundingMax);
            for (auto each : obj->GetChildren())
            {
                auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
                if (comp)
                {
                    yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(each);
                    break;
                }
            }
            AttachDebugMesh(obj, DebugMeshType::Sphere, yunuGI::Color::green());
        }
        else
        {
            obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(interactableTemplateData->pod.fBXName, &boundingMin, &boundingMax);
        }

        auto& hasAniList = ResourceManager::GetSingletonInstance().GetSkinnedFBXList();
        if (std::find(hasAniList.begin(), hasAniList.end(), interactableTemplateData->pod.fBXName) == hasAniList.end())
        {
            AdjustPickingCollider(reinterpret_cast<const Vector3f&>(boundingMin), reinterpret_cast<const Vector3f&>(boundingMax));
        }

        obj->SetParent(GetGameObject());
        currentFBX = interactableTemplateData->pod.fBXName;
    }

    void InteractableEditorInstance::ChangeTemplateData(const application::editor::InteractableData* interactableData)
    {
        ChangeTemplateData(interactableData->pod.templateData);
    }

    void InteractableEditorInstance::ChangeTemplateData(const application::editor::Interactable_TemplateData* interactableTemplateData)
    {
        if (interactableTemplateData == nullptr)
            return;
        this->interactableTemplateData = interactableTemplateData;
        ChangeResource(interactableTemplateData->pod.fBXName);
    }

    void InteractableEditorInstance::ChangeResource(const std::string& fbxName)
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

        yunuGI::Vector3 boundingMin, boundingMax;

        GameObject* obj = nullptr;

        if (interactableTemplateData->pod.fBXName == "Trigger_Cube")
        {
            obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Cube", &boundingMin, &boundingMax);
            for (auto each : obj->GetChildren())
            {
                auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
                if (comp)
                {
                    yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(each);
                    break;
                }
            }
            AttachDebugMesh(obj, DebugMeshType::Cube, yunuGI::Color::green());
        }
        else if (interactableTemplateData->pod.fBXName == "Trigger_Sphere")
        {
            obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Sphere", &boundingMin, &boundingMax);
            for (auto each : obj->GetChildren())
            {
                auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
                if (comp)
                {
                    yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(each);
                    break;
                }
            }
            AttachDebugMesh(obj, DebugMeshType::Sphere, yunuGI::Color::green());
        }
        else
        {
            obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(fbxName, &boundingMin, &boundingMax);
        }


        auto& hasAniList = ResourceManager::GetSingletonInstance().GetSkinnedFBXList();
        if (std::find(hasAniList.begin(), hasAniList.end(), interactableTemplateData->pod.fBXName) == hasAniList.end())
        {
            AdjustPickingCollider(reinterpret_cast<const Vector3f&>(boundingMin), reinterpret_cast<const Vector3f&>(boundingMax));
        }

        obj->SetParent(GetGameObject());
        currentFBX = fbxName;

        return;
    }
}
