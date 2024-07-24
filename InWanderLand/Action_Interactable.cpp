#include "Action_Interactable.h"

#include "InWanderLand.h"
#include "InteractableData.h"
#include "InteractableEditorInstance.h"

#include "Application.h"
#include "EditorLayer.h"

#include "EditorPopupManager.h"
#include "InstanceManager.h"
#include "Panel_Palette.h"
#include "InteractablePalette.h"

#include "YunutyEngine.h"
#include "Action_Interactable.h"

namespace application
{
	application::Action_InteractableShow::~Action_InteractableShow()
	{
		if (targetInteractable)
		{
			targetInteractable->RemoveObserver(this);
		}
	}
	CoroutineObject<void> Action_InteractableShow::DoAction()
	{
		if (targetInteractable->inGameInteractable)
		{
			targetInteractable->inGameInteractable->GetGameObject()->SetSelfActive(true);
		}
		co_return;
	}
	bool Action_InteractableShow::IsValid()
	{
		return (targetInteractable == nullptr) ? false : true;
	}
	void Action_InteractableShow::SetTargetInteractable(editor::InteractableData* interactable)
	{
		if (targetInteractable)
		{
			targetInteractable->RemoveObserver(this);
		}

		targetInteractable = interactable;
		if (interactable)
		{
			interactable->RegisterObserver(this);
		}
	}
	void Action_InteractableShow::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetInteractable == static_cast<editor::InteractableData*>(target))
				{
					targetInteractable = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}
	void Action_InteractableShow::ImGui_DrawDataPopup(Action_InteractableShow* data)
	{
		if (ImGui::MenuItem("SetTargetInteractable(Show)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetInteractable(Show)", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& ip = editor::palette::InteractablePalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetInteractable)
					{
						ImGui::Text(data->targetInteractable->pod.templateData->pod.fBXName.c_str());
						pp.ChangeTab("Interactable");
						ip.Reset();
						ip.SelectInteractable(data->targetInteractable->GetUUID());
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetInteractable(Show)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_InteractableShow>("SetTargetInteractable(Show)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetInteractable(Show)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}
	bool Action_InteractableShow::PreEncoding(json& data) const
	{
		return true;
	}
	bool Action_InteractableShow::PostEncoding(json& data) const
	{
		data["targetInteractable"] = targetInteractable ? UUID_To_String(targetInteractable->GetUUID()) : "nullptr";
		return true;
	}
	bool Action_InteractableShow::PreDecoding(const json& data)
	{
		return true;
	}
	bool Action_InteractableShow::PostDecoding(const json& data)
	{
		SetTargetInteractable(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::InteractableData*>(String_To_UUID(data["targetInteractable"])));
		return true;
	}
	void Action_InteractableShow::ProgressInitialize()
	{
		IAction::ProgressInitialize();
		savedActive = true;
	}
	void Action_InteractableShow::CurrentProgressSave()
	{
		IAction::CurrentProgressSave();
		if (targetInteractable->inGameInteractable)
		{
			savedActive = targetInteractable->inGameInteractable->GetGameObject()->GetSelfActive();
		}
	}
	void Action_InteractableShow::Recovery()
	{
		IAction::Recovery();
		if (!doAction && !savedActive)
		{
			targetInteractable->inGameInteractable->GetGameObject()->SetSelfActive(false);
		}
	}

	application::Action_InteractableHide::~Action_InteractableHide()
	{
		if (targetInteractable)
		{
			targetInteractable->RemoveObserver(this);
		}
	}
	CoroutineObject<void> Action_InteractableHide::DoAction()
	{
		if (targetInteractable->inGameInteractable)
		{
			targetInteractable->inGameInteractable->GetGameObject()->SetSelfActive(false);
		}
		co_return;
	}
	bool Action_InteractableHide::IsValid()
	{
		return (targetInteractable == nullptr) ? false : true;
	}
	void Action_InteractableHide::SetTargetInteractable(editor::InteractableData* interactable)
	{
		if (targetInteractable)
		{
			targetInteractable->RemoveObserver(this);
		}

		targetInteractable = interactable;
		if (interactable)
		{
			interactable->RegisterObserver(this);
		}
	}
	void Action_InteractableHide::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetInteractable == static_cast<editor::InteractableData*>(target))
				{
					targetInteractable = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}
	void Action_InteractableHide::ImGui_DrawDataPopup(Action_InteractableHide* data)
	{
		if (ImGui::MenuItem("SetTargetInteractable(Hide)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetInteractable(Hide)", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& ip = editor::palette::InteractablePalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetInteractable)
					{
						ImGui::Text(data->targetInteractable->pod.templateData->pod.fBXName.c_str());
						pp.ChangeTab("Interactable");
						ip.Reset();
						ip.SelectInteractable(data->targetInteractable->GetUUID());
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetInteractable(Hide)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_InteractableHide>("SetTargetInteractable(Hide)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetInteractable(Hide)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}
	bool Action_InteractableHide::PreEncoding(json& data) const
	{
		return true;
	}
	bool Action_InteractableHide::PostEncoding(json& data) const
	{
		data["targetInteractable"] = targetInteractable ? UUID_To_String(targetInteractable->GetUUID()) : "nullptr";
		return true;
	}
	bool Action_InteractableHide::PreDecoding(const json& data)
	{
		return true;
	}
	bool Action_InteractableHide::PostDecoding(const json& data)
	{
		SetTargetInteractable(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::InteractableData*>(String_To_UUID(data["targetInteractable"])));
		return true;
	}
	void Action_InteractableHide::ProgressInitialize()
	{
		IAction::ProgressInitialize();
		savedActive = false;
	}
	void Action_InteractableHide::CurrentProgressSave()
	{
		IAction::CurrentProgressSave();
		if (targetInteractable->inGameInteractable)
		{
			savedActive = targetInteractable->inGameInteractable->GetGameObject()->GetSelfActive();
		}
	}
	void Action_InteractableHide::Recovery()
	{
		IAction::Recovery();
		if (!doAction && savedActive)
		{
			targetInteractable->inGameInteractable->GetGameObject()->SetSelfActive(true);
		}
	}
}