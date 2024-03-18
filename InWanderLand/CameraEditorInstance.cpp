#include "CameraEditorInstance.h"

#include "InWanderLand.h"
#include "CameraData.h"
#include "EditorResourceManager.h"

namespace application::editor::palette
{
	void CameraEditorInstance::Start()
	{
		PaletteInstance::Start();
	}

	void CameraEditorInstance::Init(const application::editor::CameraData* cameraData)
	{
		Init(cameraData->pod.templateData);
	}

	void CameraEditorInstance::Init(const application::editor::Camera_TemplateData* cameraTemplateData)
	{
		this->cameraTemplateData = cameraTemplateData;
		yunuGI::Vector3 boundingMin, boundingMax;
		auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Camera", &boundingMin, &boundingMax);
		AdjustPickingCollider(reinterpret_cast<const Vector3f&>(boundingMin), reinterpret_cast<const Vector3f&>(boundingMax));
		obj->SetParent(GetGameObject());

		auto& erm = ResourceManager::GetSingletonInstance();

		for (auto each : obj->GetChildren())
		{
			auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

			if (comp)
			{
				for (int i = 0; i < comp->GetGI().GetMaterialCount(); ++i)
				{
					comp->GetGI().GetMaterial(i)->SetPixelShader(erm.GetShader("Debug_AlphaPS.cso"));
					comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,0.3,1,0.5 });
				}
			}
		}
	}
}

