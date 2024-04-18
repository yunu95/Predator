#include "Action_Camera.h"

#include "InWanderLand.h"
#include "CameraData.h"

#include "Application.h"
#include "EditorLayer.h"
#include "RTSCam.h"

#include "EditorPopupManager.h"

namespace application
{
	Action_CameraChangeView::~Action_CameraChangeView()
	{
		if (targetCam)
		{
			targetCam->RemoveObserver(this);
		}
	}

	CoroutineObject<void> Action_CameraChangeView::DoAction()
	{
		RTSCam* mainCam = static_cast<RTSCam*>(yunutyEngine::graphics::Camera::GetMainCamera());
		mainCam->SetUpdateability(false);
		auto ts = mainCam->GetTransform();
		auto startPos = ts->GetWorldPosition();
		auto startRotation = ts->GetWorldRotation();
		auto startScale = ts->GetWorldScale();
		auto& camGI = mainCam->GetGI();
		float startFov = camGI.GetVerticalFOV();
		float startCameraNear = camGI.GetNear();
		float startCameraFar = camGI.GetFar();
		float startWidth = 0;
		float startHeight = 0;
		camGI.GetResolution(&startWidth, &startHeight);
		
		double timer = 0;
		for (double timer = 0; timer < lerpTime;)
		{
			timer;
		}
		

		/// 구면 보간!
		co_return;
	}

	bool Action_CameraChangeView::IsValid()
	{
		return (targetCam != nullptr) ? true : false;
	}

	void Action_CameraChangeView::SetCamera(editor::CameraData* cam)
	{
		if (targetCam)
		{
			targetCam->RemoveObserver(this);
		}

		targetCam = cam;
		cam->RegisterObserver(this);
	}

	void Action_CameraChangeView::SetLerpTime(float lerpTime)
	{
		this->lerpTime = lerpTime;
	}

	void Action_CameraChangeView::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetCam == static_cast<editor::CameraData*>(target))
				{
					targetCam = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	void Action_CameraChangeView::ImGui_DrawDataPopup(Action_CameraChangeView* data)
	{
		if (ImGui::MenuItem("SetTargetCamera"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetCamera", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetCam)
					{
						ImGui::Text(data->targetCam->pod.name.c_str());
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetCamera");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_CameraChangeView>("SetTargetCamera", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetCamera");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetLerpTime"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float lerpTime = 0;
			lerpTime = data->lerpTime;
			editor::imgui::ShowMessageBox("SetLerpTime(ChangeView)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##LerpTime", &lerpTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetLerpTime(lerpTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(ChangeView)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(ChangeView)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_CameraChangeView::PreEncoding(json& data) const
	{
		data["lerpTime"] = lerpTime;
		return true;
	}

	bool Action_CameraChangeView::PostEncoding(json& data) const
	{
		data["targetCam"] = targetCam ? UUID_To_String(targetCam->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_CameraChangeView::PreDecoding(const json& data)
	{
		lerpTime = data["lerpTime"];
		return true;
	}

	bool Action_CameraChangeView::PostDecoding(const json& data)
	{
		targetCam = UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::CameraData*>(String_To_UUID(data["targetCam"]));
		return true;
	}

	float Action_CameraSaveView::savedFov = -1;
	float Action_CameraSaveView::savedCameraNear = -1;
	float Action_CameraSaveView::savedCameraFar = -1;
	float Action_CameraSaveView::savedWidth = -1;
	float Action_CameraSaveView::savedHeight = -1;
	yunuGI::Vector3 Action_CameraSaveView::savedPosition = yunuGI::Vector3();
	yunuGI::Quaternion Action_CameraSaveView::savedRotation = yunuGI::Quaternion();
	yunuGI::Vector3 Action_CameraSaveView::savedScale = yunuGI::Vector3();

	void Action_CameraSaveView::Clear()
	{
		savedFov = -1;
		savedCameraNear = -1;
		savedCameraFar = -1;
		savedWidth = -1;
		savedHeight = -1;
		savedPosition = yunuGI::Vector3();
		savedRotation = yunuGI::Quaternion();
		savedScale = yunuGI::Vector3();
	}

	CoroutineObject<void> Action_CameraSaveView::DoAction()
	{
		/// 데이터 static 에 저장!
		co_return;
	}

	void Action_CameraSaveView::ImGui_DrawDataPopup(Action_CameraSaveView* data)
	{

	}

	bool Action_CameraSaveView::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_CameraSaveView::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_CameraSaveView::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_CameraSaveView::PostDecoding(const json& data)
	{
		return true;
	}

	CoroutineObject<void> Action_CameraLoadView::DoAction()
	{
		/// static 녀석으로 Main 변경!
		co_return;
	}

	void Action_CameraLoadView::SetLerpTime(float lerpTime)
	{
		this->lerpTime = lerpTime;
	}

	void Action_CameraLoadView::ImGui_DrawDataPopup(Action_CameraLoadView* data)
	{
		if (ImGui::MenuItem("SetLerpTime"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float lerpTime = 0;
			lerpTime = data->lerpTime;
			editor::imgui::ShowMessageBox("SetLerpTime(LoadView)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##LerpTime", &lerpTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetLerpTime(lerpTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(LoadView)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLerpTime(LoadView)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_CameraLoadView::PreEncoding(json& data) const
	{
		data["lerpTime"] = lerpTime;
		return true;
	}

	bool Action_CameraLoadView::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_CameraLoadView::PreDecoding(const json& data)
	{
		lerpTime = data["lerpTime"];
		return true;
	}

	bool Action_CameraLoadView::PostDecoding(const json& data)
	{
		return true;
	}
}
