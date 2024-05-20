#include "LightEditorInstance.h"

#include "InWanderLand.h"
#include "LightData.h"
#include "EditorResourceManager.h"
#include "TemplateDataManager.h"

namespace application::editor::palette
{
	LightEditorInstance::~LightEditorInstance()
	{
		if (lightObj)
		{
			Scene::getCurrentScene()->DestroyGameObject(lightObj);
			fbxObj = nullptr;
			lightObj = nullptr;
		}
	}

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
				break;
			}
			case LightType::Point:
			{
				fbxType = "Sphere";
				currentLight = LightType::Point;
				lightObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
				lightObj->AddComponent<yunutyEngine::graphics::PointLight>();
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

		/// Light Range 가이드입니다.
		//switch (lightTemplateData->pod.type)
		//{
		//	case LightType::Point:
		//	{
		//		auto range = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Sphere");
		//		range->SetParent(fbxObj);
		//		range->setName("Range");

		//		for (auto each : range->GetChildren())
		//		{
		//			auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

		//			if (comp)
		//			{
		//				for (int i = 0; i < comp->GetGI().GetMaterialCount(); ++i)
		//				{
		//					comp->GetGI().GetMaterial(i)->SetPixelShader(erm.GetShader("Debug_AlphaPS.cso"));
		//					comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,0.3,1,0.1 });
		//				}
		//			}
		//		}

		//		break;
		//	}
		//	default:
		//		break;
		//}
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
				break;
			}
			case LightType::Point:
			{
				fbxType = "Sphere";
				lightObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
				lightObj->AddComponent<yunutyEngine::graphics::PointLight>();
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

		/// Light Range 가이드입니다.
		//switch (lightTemplateData->pod.type)
		//{
		//	case LightType::Point:
		//	{
		//		auto range = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Sphere");
		//		range->SetParent(fbxObj);
		//		range->setName("Range");

		//		for (auto each : range->GetChildren())
		//		{
		//			auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

		//			if (comp)
		//			{
		//				for (int i = 0; i < comp->GetGI().GetMaterialCount(); ++i)
		//				{
		//					comp->GetGI().GetMaterial(i)->SetPixelShader(erm.GetShader("Debug_AlphaPS.cso"));
		//					comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color{ 1,0.3,1,0.1 });
		//				}
		//			}
		//		}

		//		break;
		//	}
		//	default:
		//		break;
		//}

		currentLight = type;

		return;
	}

	void LightEditorInstance::ApplyLightComponent(LightData* data)
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
					lc->GetGI().SetLightDiffuseColor(*reinterpret_cast<yunuGI::Color*>(&data->pod.color));
					lc->GetGI().SetIntensity(data->pod.intensity);
					lc->GetGI().SetRange(data->pod.range);
					lc->GetGI().SetIsShadowCast(data->pod.isCast);

					auto& erm = ResourceManager::GetSingletonInstance();

					for (auto each : fbxObj->GetChildren())
					{
						auto comp = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

						if (comp)
						{
							for (int i = 0; i < comp->GetGI().GetMaterialCount(); ++i)
							{
								comp->GetGI().GetMaterial(i)->SetPixelShader(erm.GetShader("DebugPS.cso"));
								comp->GetGI().GetMaterial(i)->SetColor(*reinterpret_cast<yunuGI::Color*>(&data->pod.color));
							}
						}

						/// Light Range 가이드입니다.
						//if (each->getName() == "Range")
						//{
						//	each->GetTransform()->SetWorldScale(Vector3d(range * 2, range * 2, range * 2));

						//	for (auto reach : each->GetChildren())
						//	{
						//		auto comp = reach->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();

						//		if (comp)
						//		{
						//			for (int i = 0; i < comp->GetGI().GetMaterialCount(); ++i)
						//			{
						//				comp->GetGI().GetMaterial(i)->SetPixelShader(erm.GetShader("DebugPS.cso"));
						//				comp->GetGI().GetMaterial(i)->SetColor(yunuGI::Color(color.r, color.g, color.b, 0.1));
						//			}
						//		}
						//	}
						//}
					}
					break;
				}
				default:
					break;
			}
		}
	}

	void LightEditorInstance::ShowEditorInstance()
	{
		if (currentLight != LightType::Directional)
		{
			GetGameObject()->SetSelfActive(true);
		}

		if (lightObj)
		{
			lightObj->SetSelfActive(true);
		}
	}

	void LightEditorInstance::HideEditorInstance()
	{
		GetGameObject()->SetSelfActive(false);
		if (lightObj)
		{
			lightObj->SetSelfActive(false);
		}
	}
}

