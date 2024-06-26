#include "InWanderLand.h"
#include "LightData.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "ContentsObserveeComponent.h"

namespace application
{
	namespace editor
	{
		TemplateDataManager& LightData::templateDataManager = TemplateDataManager::GetSingletonInstance();
		LightData* LightData::editorDirectionalLight = nullptr;
		yunutyEngine::graphics::DirectionalLight* LightData::playtimeDirectionalLight = nullptr;

		LightData* LightData::GetEditorDirectionalLight()
		{
			return editorDirectionalLight;
		}

		yunutyEngine::graphics::DirectionalLight* LightData::GetPlaytimeDirectionalLight()
		{
			return playtimeDirectionalLight;
		}

		bool LightData::EnterDataFromTemplate()
		{
			// 템플릿으로부터 초기화되는 데이터들 처리 영역	

			return true;
		}

		ITemplateData* LightData::GetTemplateData()
		{
			return pod.templateData;
		}

		bool LightData::SetTemplateData(const std::string& dataName)
		{
			auto ptr = templateDataManager.GetTemplateData(dataName);
			if (ptr == nullptr)
			{
				return false;
			}

			pod.templateData = static_cast<Light_TemplateData*>(ptr);
			OnLightTypeChange(pod.templateData->pod.type);

			return true;
		}

		IEditableData* LightData::Clone() const
		{
			auto& imanager = InstanceManager::GetSingletonInstance();
			auto instance = imanager.CreateInstance(pod.templateData->GetDataKey());

			if (instance != nullptr)
			{
				static_cast<LightData*>(instance)->pod = pod;
			}

			return instance;
		}

		void LightData::OnRelocate(const Vector3d& newLoc)
		{
			pod.position.x = newLoc.x;
			pod.position.y = newLoc.y;
			pod.position.z = newLoc.z;
		}

		void LightData::OnRerotate(const Quaternion& newRot)
		{
			pod.rotation.x = newRot.x;
			pod.rotation.y = newRot.y;
			pod.rotation.z = newRot.z;
			pod.rotation.w = newRot.w;
		}

		void LightData::OnRescale(const Vector3d& newScale)
		{
			pod.scale.x = newScale.x;
			pod.scale.y = newScale.y;
			pod.scale.z = newScale.z;
		}

		void LightData::OnDataResourceChange(std::string newName)
		{
			// TemplateData 를 유지하고 Resource 만 갱신함
			if (lightInstance)
			{
				lightInstance->ChangeResource(newName);
				ApplyAsPaletteInstance();
			}
		}

		palette::PaletteInstance* LightData::ApplyAsPaletteInstance()
		{
			if (GetPaletteInstance() == nullptr)
			{
				lightInstance = Scene::getCurrentScene()->AddGameObject()->AddComponent<palette::LightEditorInstance>();
				SetPaletteInstance(lightInstance);
				lightInstance->SetEditableData(this);
				lightInstance->Init(this);
				if (pod.templateData->pod.type == LightType::Directional)
				{
					editorDirectionalLight = this;
				}
			}
			lightInstance->GetTransform()->SetWorldPosition({ pod.position.x,pod.position.y,pod.position.z });
			lightInstance->GetTransform()->SetWorldRotation({ pod.rotation.w, pod.rotation.x, pod.rotation.y, pod.rotation.z });
			lightInstance->GetTransform()->SetLocalScale({ pod.scale.x,pod.scale.y,pod.scale.z });
			lightInstance->ApplyLightComponent(this);

			return lightInstance;
		}

		void LightData::ApplyAsPlaytimeObject()
		{
			auto comp = Scene::getCurrentScene()->AddGameObject();
			comp->AddComponent<ContentsObserveeComponent>();

			switch (pod.templateData->pod.type)
			{
				case LightType::Directional:
				{
					auto light = comp->AddComponent<graphics::DirectionalLight>();
					playtimeDirectionalLight = light;
					//yunuGI::Color color{ 0.67 * 0.0015,0.65 * 0.0015,0.61 * 0.0015,1.f };
					//yunuGI::Color color{ 0.0015,0.0015,0.0015,1.f };
					//light->GetGI().SetLightDiffuseColor(color);
					//light->GetGI().SetIntensity(pod.intensity);
					break;
				}
				case LightType::Point:
				{
					auto light = comp->AddComponent<graphics::PointLight>();
					light->GetGI().SetLightDiffuseColor(*reinterpret_cast<yunuGI::Color*>(&pod.color));
					light->GetGI().SetIntensity(pod.intensity);
					light->GetGI().SetRange(pod.range);
					light->GetGI().SetIsShadowCast(pod.isCast);
					break;
				}
				default:
					break;
			}

			comp->GetTransform()->SetWorldPosition(Vector3d(pod.position.x, pod.position.y, pod.position.z));
			comp->GetTransform()->SetWorldRotation(Quaternion(pod.rotation.w, pod.rotation.x, pod.rotation.y, pod.rotation.z));
			comp->GetTransform()->SetLocalScale(Vector3d(pod.scale.x, pod.scale.y, pod.scale.z));
		}

		void LightData::OnLightTypeChange(LightType type)
		{
			if (lightInstance)
			{
				lightInstance->ChangeLight(type);
				ApplyAsPaletteInstance();
			}
		}

		bool LightData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Light>>(pod, data["POD"]);

			return true;
		}

		bool LightData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Light>>(pod, data["POD"]);

			return true;
		}

		bool LightData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Light>>(pod, data["POD"]);

			return true;
		}

		bool LightData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Light>>(pod, data["POD"]);
			EnterDataFromGlobalConstant();
#ifdef EDITOR
			ApplyAsPaletteInstance();
#endif
			return true;
		}

		LightData::LightData()
			: pod()
		{

		}

		LightData::LightData(const std::string& name)
			: pod()
		{
			pod.templateData = static_cast<Light_TemplateData*>(templateDataManager.GetTemplateData(name));
			EnterDataFromTemplate();
			EnterDataFromGlobalConstant();
		}

		LightData::LightData(const LightData& prototype)
			: pod(prototype.pod)
		{

		}

		LightData& LightData::operator=(const LightData& prototype)
		{
			IEditableData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
