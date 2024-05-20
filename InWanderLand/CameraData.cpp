#include "InWanderLand.h"
#include "CameraData.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "EditorCameraManager.h"

#include "RTSCam.h"
#include "Dotween.h"

#include "SkillPreviewSystem.h"
#include "PlayerController.h"
#include "TacticModeSystem.h"
#include "GameManager.h"
#include "PlayTimeRegionManager.h"
#include "CursorDetector.h"

namespace application
{
	namespace editor
	{
		TemplateDataManager& CameraData::templateDataManager = TemplateDataManager::GetSingletonInstance();

		CameraData::~CameraData()
		{
			CameraManager::GetSingletonInstance().EraseCamera(this);
		}

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
			cameraInstance->ApplyCamComponent(this);
			return cameraInstance;
		}

		void CameraData::ApplyAsPlaytimeObject()
		{
			if (pod.isMain == false)
			{
				return;
			}

			auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			auto camComp = camObj->AddComponent<RTSCam>();
			PlayTimeRegionManager::Instance().SetCameraDotween(camObj->AddComponent<Dotween>());
			InputManager::Instance().rtscam = camComp;
			GameManager::Instance().rtscam = camComp;
			camObj->GetTransform()->SetWorldPosition({ pod.position.x,pod.position.y,pod.position.z });
			camObj->GetTransform()->SetWorldRotation({ pod.rotation.w, pod.rotation.x, pod.rotation.y, pod.rotation.z });
			camObj->GetTransform()->SetWorldScale({ pod.scale.x,pod.scale.y,pod.scale.z });

			camComp->GetGI().SetVerticalFOV(pod.vertical_FOV);
			camComp->GetGI().SetNear(pod.dis_Near);
			camComp->GetGI().SetFar(pod.dis_Far);
			camComp->GetGI().SetResolution(pod.res_Width, pod.res_Height);

			/// Main 으로 등록해야 추후 Editor Camera 로직에서 처리 가능합니다.
			camComp->SetCameraMain();

			auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

			auto sphereMesh = rsrcMgr->GetMesh(L"Sphere");
			auto mouseCursorObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			auto mouseCursorMesh = mouseCursorObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
			mouseCursorMesh->GetGI().SetMesh(sphereMesh);
			mouseCursorMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 0, 1 });

			auto mouseCursorColliderComponent = mouseCursorObject->AddComponent<physics::SphereCollider>();
			mouseCursorColliderComponent->SetRadius(UNIT_LENGTH * 0.5f);
			//mouseCursorObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);

			auto cursorDetectorComponent = mouseCursorObject->AddComponent<CursorDetector>();

			PlayerController::Instance().m_cursorDetector = cursorDetectorComponent;
			TacticModeSystem::Instance().m_cursorDetector = cursorDetectorComponent;

			camComp->groundHoveringClickCallback = [=](Vector3d pos)
			{
				mouseCursorObject->GetTransform()->SetWorldPosition(pos);
				//SkillPreviewSystem::Instance().SetCurrentMousPosition(pos);
			};

			PlayerController::Instance().SetMovingSystemComponent(camComp);
			TacticModeSystem::Instance().SetMovingSystemComponent(camComp);
		}

		yunutyEngine::graphics::Camera* CameraData::GetCameraComponent()
		{
			if (cameraInstance)
			{
				return cameraInstance->GetCameraComponent();
			}

			return nullptr;
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
			CameraManager::GetSingletonInstance().RegisterCamera(this);
		}

		CameraData::CameraData(const std::string& name)
			: pod()
		{
			pod.templateData = static_cast<Camera_TemplateData*>(templateDataManager.GetTemplateData(name));
			EnterDataFromTemplate();
			EnterDataFromGlobalConstant();
			CameraManager::GetSingletonInstance().RegisterCamera(this);
		}

		CameraData::CameraData(const CameraData& prototype)
			: pod(prototype.pod)
		{
			CameraManager::GetSingletonInstance().RegisterCamera(this);
		}

		CameraData& CameraData::operator=(const CameraData& prototype)
		{
			IEditableData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
