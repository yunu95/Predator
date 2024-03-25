#include "CameraEditorInstance.h"

#include "InWanderLand.h"
#include "CameraData.h"
#include "EditorResourceManager.h"
#include "EditorCameraManager.h"

namespace application::editor::palette
{
	CameraEditorInstance::~CameraEditorInstance()
	{
		if (camObj)
		{
			Scene::getCurrentScene()->DestroyGameObject(camObj);
			fbxObj = nullptr;
			camObj = nullptr;
		}
	}

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
		fbxObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Camera", &boundingMin, &boundingMax);
		AdjustPickingCollider(reinterpret_cast<const Vector3f&>(boundingMin), reinterpret_cast<const Vector3f&>(boundingMax));
		fbxObj->SetParent(GetGameObject());

		auto& erm = ResourceManager::GetSingletonInstance();

		for (auto each : fbxObj->GetChildren())
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

		camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		camObj->AddComponent<yunutyEngine::graphics::Camera>();
	}

	void CameraEditorInstance::ApplyCamComponent(CameraData* data)
	{
		if (camObj)
		{
			camObj->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
			camObj->GetTransform()->SetWorldRotation(GetTransform()->GetWorldRotation());
			camObj->GetTransform()->SetLocalScale(GetTransform()->GetLocalScale());

			auto camComp = camObj->GetComponent<yunutyEngine::graphics::Camera>();

			if (camComp)
			{
				camComp->GetGI().SetVerticalFOV(data->pod.vertical_FOV);
				camComp->GetGI().SetNear(data->pod.dis_Near);
				camComp->GetGI().SetFar(data->pod.dis_Far);
				camComp->GetGI().SetResolution(data->pod.res_Width, data->pod.res_Height);
			}

			if (data->pod.isMain)
			{
				CameraManager::GetSingletonInstance().SetMainCam(data);
			}
		}
	}

	yunutyEngine::graphics::Camera* CameraEditorInstance::GetCameraComponent()
	{
		if (camObj)
		{
			return camObj->GetComponent<yunutyEngine::graphics::Camera>();
		}

		return nullptr;
	}

	void CameraEditorInstance::SetAsMain()
	{
		if (camObj)
		{
			auto camComp = camObj->GetComponent<yunutyEngine::graphics::Camera>();

			if (camComp)
			{
				camComp->GetGI().SetAsMain();
			}
		}
	}

	void CameraEditorInstance::ShowEditorInstance()
	{
		GetGameObject()->SetSelfActive(true);
		if (camObj)
		{
			camObj->SetSelfActive(true);
		}
	}

	void CameraEditorInstance::HideEditorInstance()
	{
		GetGameObject()->SetSelfActive(false);
		if (camObj)
		{
			camObj->SetSelfActive(false);
		}
	}
}

