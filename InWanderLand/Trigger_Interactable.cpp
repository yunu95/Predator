#include "Trigger_Interactable.h"
#include "InteractableData.h"

#include "IInteractableComponent.h"

#include "Application.h"
#include "EditorLayer.h"
#include "Panel_Palette.h"
#include "InteractablePalette.h"

#include "EditorPopupManager.h"

namespace application
{
	Trigger_InteractableTriggerEnter::~Trigger_InteractableTriggerEnter()
	{
		if (targetInteractable)
		{
			targetInteractable->RemoveObserver(this);
		}
	}

	void Trigger_InteractableTriggerEnter::LinkCallback()
	{
		assert(targetInteractable && targetInteractable->inGameInteractable);
		targetInteractable->inGameInteractable->OnEnterCallBack.push_back([=]() { PullTrigger(); });
	}

	void Trigger_InteractableTriggerEnter::SetInteractable(editor::InteractableData* interactable)
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

	bool Trigger_InteractableTriggerEnter::IsValid()
	{
		return (targetInteractable != nullptr) ? true : false;
	}

	void Trigger_InteractableTriggerEnter::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
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

	void Trigger_InteractableTriggerEnter::ImGui_DrawDataPopup(Trigger_InteractableTriggerEnter* data)
	{
		if (ImGui::MenuItem("SetTargetInteractable(TriggerEnter)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetInteractable(TriggerEnter)", [data]()
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
						editor::imgui::CloseMessageBox("SetTargetInteractable(TriggerEnter)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Trigger_InteractableTriggerEnter>("SetTargetInteractable(TriggerEnter)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetInteractable(TriggerEnter)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Trigger_InteractableTriggerEnter::PreEncoding(json& data) const
	{
		return true;
	}

	bool Trigger_InteractableTriggerEnter::PostEncoding(json& data) const
	{
		data["targetInteractable"] = targetInteractable ? UUID_To_String(targetInteractable->GetUUID()) : "nullptr";
		return true;
	}

	bool Trigger_InteractableTriggerEnter::PreDecoding(const json& data)
	{
		return true;
	}

	bool Trigger_InteractableTriggerEnter::PostDecoding(const json& data)
	{
		SetInteractable(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::InteractableData*>(String_To_UUID(data["targetInteractable"])));
		return true;
	}

	Trigger_InteractableTriggerExit::~Trigger_InteractableTriggerExit()
	{
		if (targetInteractable)
		{
			targetInteractable->RemoveObserver(this);
		}
	}

	void Trigger_InteractableTriggerExit::LinkCallback()
	{
		assert(targetInteractable && targetInteractable->inGameInteractable);
		targetInteractable->inGameInteractable->OnExitCallBack.push_back([=]() { PullTrigger(); });
	}

	void Trigger_InteractableTriggerExit::SetInteractable(editor::InteractableData* interactable)
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

	bool Trigger_InteractableTriggerExit::IsValid()
	{
		return (targetInteractable != nullptr) ? true : false;
	}

	void Trigger_InteractableTriggerExit::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
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

	void Trigger_InteractableTriggerExit::ImGui_DrawDataPopup(Trigger_InteractableTriggerExit* data)
	{
		if (ImGui::MenuItem("SetTargetInteractable(TriggerExit)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetInteractable(TriggerExit)", [data]()
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
						editor::imgui::CloseMessageBox("SetTargetInteractable(TriggerExit)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Trigger_InteractableTriggerExit>("SetTargetInteractable(TriggerExit)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetInteractable(TriggerExit)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Trigger_InteractableTriggerExit::PreEncoding(json& data) const
	{
		return true;
	}

	bool Trigger_InteractableTriggerExit::PostEncoding(json& data) const
	{
		data["targetInteractable"] = targetInteractable ? UUID_To_String(targetInteractable->GetUUID()) : "nullptr";
		return true;
	}

	bool Trigger_InteractableTriggerExit::PreDecoding(const json& data)
	{
		return true;
	}

	bool Trigger_InteractableTriggerExit::PostDecoding(const json& data)
	{
		SetInteractable(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::InteractableData*>(String_To_UUID(data["targetInteractable"])));
		return true;
	}
}
