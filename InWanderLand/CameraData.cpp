#include "InWanderLand.h"
#include "CameraData.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		TemplateDataManager& CameraData::templateDataManager = TemplateDataManager::GetSingletonInstance();

		bool CameraData::EnterDataFromTemplate()
		{
			// 템플릿으로부터 초기화되는 데이터들 처리 영역	

			return true;
		}

		ITemplateData* CameraData::GetTemplateData()
		{
			return pod.templateData;
		}

		bool CameraData::SetTemplateData(const std::string& dataName)
		{
			auto ptr = templateDataManager.GetTemplateData(dataName);
			if (ptr == nullptr)
			{
				return false;
			}

			pod.templateData = static_cast<Camera_TemplateData*>(ptr);

			return true;
		}

		IEditableData* CameraData::Clone() const
		{
			auto& imanager = InstanceManager::GetSingletonInstance();
			auto instance = imanager.CreateInstance(pod.templateData->GetDataKey());

			if (instance != nullptr)
			{
				static_cast<CameraData*>(instance)->pod = pod;
			}

			return instance;
		}

		void CameraData::OnRelocate(const Vector3d& newLoc)
		{
			pod.position.x = newLoc.x;
			pod.position.y = newLoc.y;
			pod.position.z = newLoc.z;
		}

		void CameraData::OnRerotate(const Quaternion& newRot)
		{
			pod.rotation.x = newRot.x;
			pod.rotation.y = newRot.y;
			pod.rotation.z = newRot.z;
			pod.rotation.w = newRot.w;
		}

		void CameraData::OnRescale(const Vector3d& newScale)
		{
			pod.scale.x = newScale.x;
			pod.scale.y = newScale.y;
			pod.scale.z = newScale.z;
		}

		void CameraData::OnDataResourceChange(std::string newName)
		{
			// TemplateData 를 유지하고 Resource 만 갱신함
			if (cameraInstance)
			{
				cameraInstance->ChangeResource(newName);
				ApplyAsPaletteInstance();
			}
		}

		palette::PaletteInstance* CameraData::ApplyAsPaletteInstance()
		{
			if (GetPaletteInstance() == nullptr)
			{
				cameraInstance = Scene::getCurrentScene()->AddGameObject()->AddComponent<palette::CameraEditorInstance>();
				SetPaletteInstance(cameraInstance);
				cameraInstance->SetEditableData(this);
				cameraInstance->Init(this);
			}
			cameraInstance->GetTransform()->SetWorldPosition({ pod.position.x,pod.position.y,pod.position.z });
			cameraInstance->GetTransform()->SetWorldRotation({ pod.rotation.w, pod.rotation.x, pod.rotation.y, pod.rotation.z });
			cameraInstance->GetTransform()->SetLocalScale({ pod.scale.x,pod.scale.y,pod.scale.z });
			return cameraInstance;
		}

		void CameraData::ApplyAsPlaytimeObject()
		{

		}

		bool CameraData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Camera>>(pod, data["POD"]);

			return true;
		}

		bool CameraData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Camera>>(pod, data["POD"]);

			return true;
		}

		bool CameraData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Camera>>(pod, data["POD"]);

			return true;
		}

		bool CameraData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Camera>>(pod, data["POD"]);
			EnterDataFromGlobalConstant();
#ifdef EDITOR
			ApplyAsPaletteInstance();
#endif
			return true;
		}

		CameraData::CameraData()
			: pod()
		{

		}

		CameraData::CameraData(const std::string& name)
			: pod()
		{
			pod.templateData = static_cast<Camera_TemplateData*>(templateDataManager.GetTemplateData(name));
			EnterDataFromTemplate();
			EnterDataFromGlobalConstant();
		}

		CameraData::CameraData(const CameraData& prototype)
			: pod(prototype.pod)
		{

		}

		CameraData& CameraData::operator=(const CameraData& prototype)
		{
			IEditableData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
