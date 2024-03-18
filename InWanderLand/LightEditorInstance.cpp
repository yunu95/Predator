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
				break;
			}
			case LightType::Point:
			{
				fbxType = "Sphere";
				currentLight = LightType::Point;
				break;
			}
			default:
				break;
		}

		yunuGI::Vector3 boundingMin, boundingMax;
		auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(fbxType, &boundingMin, &boundingMax);
		AdjustPickingCollider(reinterpret_cast<const Vector3f&>(boundingMin), reinterpret_cast<const Vector3f&>(boundingMax));
		obj->setName("LightFBX");
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

		for (auto& each : GetGameObject()->GetChildren())
		{
			if (each->getName() == "LightFBX")
			{
				yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(each);
				break;
			}
		}

		std::string fbxType;

		switch (type)
		{
			case LightType::Directional:
			{
				fbxType = "Directional";
				break;
			}
			case LightType::Point:
			{
				fbxType = "Sphere";
				break;
			}
			default:
				break;
		}

		yunuGI::Vector3 boundingMin, boundingMax;
		auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(fbxType, &boundingMin, &boundingMax);
		AdjustPickingCollider(reinterpret_cast<const Vector3f&>(boundingMin), reinterpret_cast<const Vector3f&>(boundingMax));
		obj->setName("LightFBX");
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

		currentLight = type;

		return;
	}
}

