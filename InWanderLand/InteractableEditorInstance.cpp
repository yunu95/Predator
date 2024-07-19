#include "InWanderLand.h"
#include "DebugMeshes.h"
#include "InteractableEditorInstance.h"
#include "InteractableData.h"
#include "EditorResourceManager.h"
#include "DebugMeshes.h"
#include "Interactable_UI.h"
#include "UIManager.h"

namespace application::editor::palette
{
	void InteractableEditorInstance::Start()
	{
		PaletteInstance::Start();
		uiObject = GetGameObject()->AddGameObject();
		uiObject->AddComponent<Interactable_UI>();
		uiObject->SetSelfActive(false);
	}

	void InteractableEditorInstance::Update()
	{
		auto data = static_cast<InteractableData*>(GetEditableData());
		if (data == nullptr)
		{
			return;
		}

		/// UI 시스템 개선과 함께 개선이 필요한 영역
		if (uiObject)
		{
			if (data->pod.templateData->pod.activeInteractable)
			{
				uiObject->SetSelfActive(true);
			}
			else
			{
				uiObject->SetSelfActive(false);
			}

			auto uiComp = uiObject->GetComponent<Interactable_UI>();
			uiComp->SetUI(data->pod.guideUI);
			uiComp->SetUIWidth(data->pod.ui_Width);
			uiComp->SetUIHeight(data->pod.ui_Height);
			uiComp->UpdateUI();

			auto scale = GetGameObject()->GetTransform()->GetWorldScale();
			auto pos = UIManager::Instance().GetUIPosFromWorld(GetGameObject()->GetTransform()->GetWorldPosition());
			uiObject->GetTransform()->SetLocalScale(Vector3d(1 / scale.x, 1 / scale.y, 1 / scale.z));
			uiObject->GetTransform()->SetWorldPosition(Vector3d((pos.x + data->pod.uiOffset.x) / scale.x, (pos.y + data->pod.uiOffset.y) / scale.y, 0));
		}
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

		if (interactableTemplateData->pod.fBXName == "Trigger_Cube" || interactableTemplateData->pod.fBXName == "Barricade_Cube")
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
		else if (interactableTemplateData->pod.fBXName == "Trigger_Sphere" || interactableTemplateData->pod.fBXName == "Barricade_Sphere")
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
		else if (interactableTemplateData->pod.fBXName == "SM_Spike01")
		{
			obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(interactableTemplateData->pod.fBXName, &boundingMin, &boundingMax);
			yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_SpikeFrame")->SetParent(obj);
		}
		else
		{
			obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(interactableTemplateData->pod.fBXName, &boundingMin, &boundingMax);
		}

		AdjustPickingCollider(reinterpret_cast<const Vector3f&>(boundingMin), reinterpret_cast<const Vector3f&>(boundingMax));

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

		if (interactableTemplateData->pod.fBXName == "Trigger_Cube" || interactableTemplateData->pod.fBXName == "Barricade_Cube")
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
		else if (interactableTemplateData->pod.fBXName == "Trigger_Sphere" || interactableTemplateData->pod.fBXName == "Barricade_Sphere")
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
		else if (interactableTemplateData->pod.fBXName == "SM_Spike01")
		{
			obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(interactableTemplateData->pod.fBXName, &boundingMin, &boundingMax);
			yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_SpikeFrame")->SetParent(obj);
		}
		else
		{
			obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(fbxName, &boundingMin, &boundingMax);
		}

		AdjustPickingCollider(reinterpret_cast<const Vector3f&>(boundingMin), reinterpret_cast<const Vector3f&>(boundingMax));

		obj->SetParent(GetGameObject());
		currentFBX = fbxName;

		return;
	}
}
