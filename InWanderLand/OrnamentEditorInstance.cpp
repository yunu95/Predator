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
        mesh = GetGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        Init(ornamentData->pod.templateData);
    }

    void OrnamentEditorInstance::Init(const application::editor::Ornament_TemplateData* ornamentTemplateData)
    {
        this->ornamentTemplateData = ornamentTemplateData;
        auto resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
        
        auto meshData = resourceManager->GetMesh(std::wstring(ornamentTemplateData->pod.meshKey.begin(), ornamentTemplateData->pod.meshKey.end()));
        if (meshData == nullptr)
        {
            meshData = resourceManager->GetMesh(L"Sphere");
        }

        mesh->GetGI().SetMesh(meshData);

        for (int i = 0; i < ornamentTemplateData->pod.materialKey.size(); i++)
        {
            auto rsc = resourceManager->GetMaterial(std::wstring(ornamentTemplateData->pod.materialKey[i].begin(), ornamentTemplateData->pod.materialKey[i].end()));
            if (rsc == nullptr)
                break;

            mesh->GetGI().SetMaterial(i, rsc);
        }
    }

    void OrnamentEditorInstance::ResizeMesh(yunuGI::Vector3 scale)
    {
        if (mesh)
        {
            mesh->GetTransform()->SetLocalScale({scale.x, scale.y, scale.z});
        }
    }
}

