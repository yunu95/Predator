#include "LightEditorInstance.h"

#include "InWanderLand.h"
#include "LightData.h"
#include "EditorResourceManager.h"

namespace application::editor::palette
{
	void LightEditorInstance::Start()
	{
		PaletteInstance::Start();
	}

	void LightEditorInstance::Init(const application::editor::LightData* lightData)
	{
		Init(lightData->pod.templateData);
	}

	void LightEditorInstance::Init(const application::editor::Light_TemplateData* lightTemplateData)
	{
		this->lightTemplateData = lightTemplateData;

		std::string fbxType;

		switch (lightTemplateData->pod.type)
		{
			case LightType::Directional:
			{
				fbxType = "Directional";
				currentLight = LightType::Directional;
				lightObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
				lightObj->AddComponent<yunutyEngine::graphics::DirectionalLight>();
				lightObj->SetParent(GetGameObject());
				break;
			}
			case LightType::Point:
			{
				fbxType = "Sphere";
				currentLight = LightType::Point;
				lightObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
				lightObj->AddComponent<yunutyEngine::graphics::PointLight>();
				lightObj->SetParent(GetGameObject());
				break;
			}
			default:
				break;
		}

		yunuGI::Vector3 boundingMin, boundingMax;
		fbxObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(fbxType, &boundingMin, &boundingMax);
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
					comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,0.3,1,0.1 });
				}
			}
		}
	}

	void LightEditorInstance::ChangeTemplateData(const application::editor::LightData* lightData)
	{
		ChangeTemplateData(lightData->pod.templateData);
	}

	void LightEditorInstance::ChangeTemplateData(const application::editor::Light_TemplateData* lightTemplateData)
	{
		if (this->lightTemplateData == lightTemplateData)
			return;
		this->lightTemplateData = lightTemplateData;

		auto type = TemplateDataManager::GetSingletonInstance().GetDataKey(lightTemplateData);
		if (type == "Directional_Light")
		{
			ChangeLight(LightType::Directional);
		}
		else if (type == "Point_Light")
		{
			ChangeLight(LightType::Point);
		}
	}

	void LightEditorInstance::ChangeLight(LightType type)
	{
		if (currentLight == type)
			return;

		yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(fbxObj);
		yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(lightObj);

		std::string fbxType;

		switch (type)
		{
			case LightType::Directional:
			{
				fbxType = "Directional";
				lightObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
				lightObj->AddComponent<yunutyEngine::graphics::DirectionalLight>();
				lightObj->SetParent(GetGameObject());
				break;
			}
			case LightType::Point:
			{
				fbxType = "Sphere";
				lightObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
				lightObj->AddComponent<yunutyEngine::graphics::PointLight>();
				lightObj->SetParent(GetGameObject());
				break;
			}
			default:
				break;
		}

		yunuGI::Vector3 boundingMin, boundingMax;
		fbxObj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(fbxType, &boundingMin, &boundingMax);
		AdjustPickingCollider(reinterpret_cast<const Vector3f&>(boundingMin), reinterpret_cast<const Vector3f&>(boundingMax));
		fbxObj->setName("LightFBX");
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
					comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,0.3,1,0.1 });
				}
			}
		}

		currentLight = type;

		return;
	}

	void LightEditorInstance::ApplyLightComponent(float range, yunuGI::Color color)
	{
		if (lightObj)
		{
			lightObj->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
			lightObj->GetTransform()->SetWorldRotation(GetTransform()->GetWorldRotation());
			lightObj->GetTransform()->SetLocalScale(GetTransform()->GetWorldScale());
			
			switch (currentLight)
			{
				case application::editor::LightType::Point:
				{
					auto lc = lightObj->GetComponent<yunutyEngine::graphics::PointLight>();
					lc->GetGI().SetRange(range);
					lc->GetGI().SetLightDiffuseColor(color);
					break;
				}
				default:
					break;
			}
		}
	}

	void LightEditorInstance::ShowEditorInstance()
	{
		if (fbxObj && currentLight != LightType::Directional)
		{
			fbxObj->SetSelfActive(true);
		}
	}

	void LightEditorInstance::HideEditorInstance()
	{
		if (fbxObj)
		{
			fbxObj->SetSelfActive(false);
		}
	}
}

