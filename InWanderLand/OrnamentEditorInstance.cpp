#include "OrnamentEditorInstance.h"

#include "InWanderLand.h"
#include "OrnamentData.h"

#include "EditorResourceManager.h"

namespace application::editor::palette
{
    void OrnamentEditorInstance::Start()
    {
        PaletteInstance::Start();
    }

    void OrnamentEditorInstance::Update()
    {
        if (ornamentData->pod.isBillBoard)
        {
            if (auto cam = graphics::Camera::GetMainCamera())
            {
                GetTransform()->SetWorldRotation(cam->GetTransform()->GetWorldRotation());
            }
        }
        else
        {
            Component::Update();
        }
    }

    void OrnamentEditorInstance::Init(const application::editor::OrnamentData* ornamentData)
    {
        this->ornamentData = ornamentData;
        Init(ornamentData->pod.templateData);

        if (ornamentData->pod.isGuide)
        {
            ChangeGuideInstance();
        }
    }

    void OrnamentEditorInstance::Init(const application::editor::Ornament_TemplateData* ornamentTemplateData)
    {
        this->ornamentTemplateData = ornamentTemplateData;
        yunuGI::Vector3 boundingMin, boundingMax;
        auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(ornamentTemplateData->pod.staticFBXName, &boundingMin, &boundingMax);
        for (auto child : obj->GetChildrenRecursively())
        {
            auto staticMeshRenderer = child->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
            if (staticMeshRenderer == nullptr)
                continue;

            if (this->ornamentData->pod.LightMapIndex != -1)
            {
                staticMeshRenderer->GetGI().SetLightMapUVIndex(this->ornamentData->pod.LightMapIndex);
                staticMeshRenderer->GetGI().SetLightMapUVOffset(this->ornamentData->pod.LightMapScaleOffset[2], this->ornamentData->pod.LightMapScaleOffset[3]);
                staticMeshRenderer->GetGI().SetLightMapUVScaling(this->ornamentData->pod.LightMapScaleOffset[0], this->ornamentData->pod.LightMapScaleOffset[1]);
            }
        }
        AdjustPickingCollider(reinterpret_cast<const Vector3f&>(boundingMin), reinterpret_cast<const Vector3f&>(boundingMax));
        currentFBX = ornamentTemplateData->pod.staticFBXName;
        obj->SetParent(GetGameObject());
    }

    void OrnamentEditorInstance::ChangeTemplateData(const application::editor::OrnamentData* ornamentData)
    {
        ChangeTemplateData(ornamentData->pod.templateData);
    }

    void OrnamentEditorInstance::ChangeTemplateData(const application::editor::Ornament_TemplateData* ornamentTemplateData)
    {
        if (this->ornamentTemplateData == ornamentTemplateData)
            return;
        this->ornamentTemplateData = ornamentTemplateData;
        ChangeResource(ornamentTemplateData->pod.staticFBXName);
    }

    void OrnamentEditorInstance::ChangeResource(const std::string& fbxName)
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

        yunuGI::Vector3 boundingMin, boundingMax;
        auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(fbxName, &boundingMin, &boundingMax);
        AdjustPickingCollider(reinterpret_cast<const Vector3f&>(boundingMin), reinterpret_cast<const Vector3f&>(boundingMax));
        currentFBX = fbxName;

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

        return;
    }

    void OrnamentEditorInstance::ShowEditorInstance()
    {
        if (isGuide)
        {
            return;
        }

        if (ornamentData->tookAction && !GetGameObject()->GetActive())
        {
            actionActiveFlag = false;
        }

        GetGameObject()->SetSelfActive(true);
    }

    void OrnamentEditorInstance::HideEditorInstance()
    {
        if (isGuide)
        {
            return;
        }

        if (ornamentData->tookAction && !actionActiveFlag)
        {
            actionActiveFlag = true;
            GetGameObject()->SetSelfActive(false);
            return;
        }

        /// Ornament 는 Ingame 에 그대로 사용되므로, 혹시나 꺼져있던 Ornament 를 활성화합니다.
        GetGameObject()->SetSelfActive(true);
    }

    const application::editor::OrnamentData* OrnamentEditorInstance::GetOrnamentData()
    {
        return ornamentData;
    }

    void OrnamentEditorInstance::ChangeGuideInstance()
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
}

