#include "OrnamentEditorInstance.h"

#include "InWanderLand.h"
#include "OrnamentData.h"

namespace application::editor::palette
{
    void OrnamentEditorInstance::Start()
    {
        PaletteInstance::Start();
    }

    void OrnamentEditorInstance::Init(const application::editor::OrnamentData* ornamentData)
    {
        this->ornamentData = ornamentData;
        Init(ornamentData->pod.templateData);
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
            staticMeshRenderer->GetGI().SetLightMapUVIndex(this->ornamentData->pod.LightMapIndex);
            staticMeshRenderer->GetGI().SetLightMapUVOffset(this->ornamentData->pod.LightMapScaleOffset[2], this->ornamentData->pod.LightMapScaleOffset[3]);
            staticMeshRenderer->GetGI().SetLightMapUVScaling(this->ornamentData->pod.LightMapScaleOffset[0], this->ornamentData->pod.LightMapScaleOffset[1]);
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
        // TemplateData �� �����ϰ� Resource �� ������
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

        return;
    }
}

