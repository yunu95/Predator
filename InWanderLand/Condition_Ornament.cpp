#include "Condition_Ornament.h"

#include "InWanderLand.h"
#include "OrnamentData.h"
#include "OrnamentEditorInstance.h"

#include "Application.h"
#include "EditorLayer.h"
#include "Panel_Palette.h"
#include "OrnamentPalette.h"

#include "EditorPopupManager.h"
#include "InstanceManager.h"

#include "YunutyEngine.h"

namespace application
{
	Condition_OrnamentShow::~Condition_OrnamentShow()
	{
		if (targetOrnament)
		{
			targetOrnament->RemoveObserver(this);
		}
	}

	bool Condition_OrnamentShow::IsConditionMet()
	{
		return targetOrnament->GetPaletteInstance()->GetGameObject()->GetActive();
	}

	bool Condition_OrnamentShow::IsValid()
	{
		return (targetOrnament == nullptr) ? false : true;
	}

	void Condition_OrnamentShow::ImGui_DrawDataPopup(Condition_OrnamentShow* data)
	{
		if (ImGui::MenuItem("SetTargetOrnament(Condition Show)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetOrnament(Condition Show)", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& op = editor::palette::OrnamentPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetOrnament)
					{
						ImGui::Text(data->targetOrnament->pod.templateData->pod.staticFBXName.c_str());
						pp.ChangeTab("Ornament");
						op.Reset();
						op.SelectOrnamentInstance(data->targetOrnament);
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetOrnament(Condition Show)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Condition_OrnamentShow>("SetTargetOrnament(Condition Show)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetOrnament(Condition Show)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	void Condition_OrnamentShow::SetTargetOrnament(editor::OrnamentData* ornament)
	{
		if (targetOrnament)
		{
			targetOrnament->RemoveObserver(this);
		}

		targetOrnament = ornament;
		if (ornament)
		{
			ornament->RegisterObserver(this);
		}
	}

	void Condition_OrnamentShow::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetOrnament == static_cast<editor::OrnamentData*>(target))
				{
					targetOrnament = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	bool Condition_OrnamentShow::PreEncoding(json& data) const
	{
		return true;
	}

	bool Condition_OrnamentShow::PostEncoding(json& data) const
	{
		data["targetOrnament"] = targetOrnament ? UUID_To_String(targetOrnament->GetUUID()) : "nullptr";
		return true;
	}

	bool Condition_OrnamentShow::PreDecoding(const json& data)
	{
		return true;
	}

	bool Condition_OrnamentShow::PostDecoding(const json& data)
	{
		SetTargetOrnament(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::OrnamentData*>(String_To_UUID(data["targetOrnament"])));
		return true;
	}

	Condition_OrnamentHide::~Condition_OrnamentHide()
	{
		if (targetOrnament)
		{
			targetOrnament->RemoveObserver(this);
		}
	}

	bool Condition_OrnamentHide::IsConditionMet()
	{
		return !targetOrnament->GetPaletteInstance()->GetGameObject()->GetActive();
	}

	bool Condition_OrnamentHide::IsValid()
	{
		return (targetOrnament == nullptr) ? false : true;
	}

	void Condition_OrnamentHide::ImGui_DrawDataPopup(Condition_OrnamentHide* data)
	{
		if (ImGui::MenuItem("SetTargetOrnament(Condition Hide)"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetTargetOrnament(Condition Hide)", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& op = editor::palette::OrnamentPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetOrnament)
					{
						ImGui::Text(data->targetOrnament->pod.templateData->pod.staticFBXName.c_str());
						pp.ChangeTab("Ornament");
						op.Reset();
						op.SelectOrnamentInstance(data->targetOrnament);
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetOrnament(Condition Hide)");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Condition_OrnamentHide>("SetTargetOrnament(Condition Hide)", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetOrnament(Condition Hide)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	void Condition_OrnamentHide::SetTargetOrnament(editor::OrnamentData* ornament)
	{
		if (targetOrnament)
		{
			targetOrnament->RemoveObserver(this);
		}

		targetOrnament = ornament;
		if (ornament)
		{
			ornament->RegisterObserver(this);
		}
	}

	void Condition_OrnamentHide::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetOrnament == static_cast<editor::OrnamentData*>(target))
				{
					targetOrnament = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	bool Condition_OrnamentHide::PreEncoding(json& data) const
	{
		return true;
	}

	bool Condition_OrnamentHide::PostEncoding(json& data) const
	{
		data["targetOrnament"] = targetOrnament ? UUID_To_String(targetOrnament->GetUUID()) : "nullptr";
		return true;
	}

	bool Condition_OrnamentHide::PreDecoding(const json& data)
	{
		return true;
	}

	bool Condition_OrnamentHide::PostDecoding(const json& data)
	{
		SetTargetOrnament(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::OrnamentData*>(String_To_UUID(data["targetOrnament"])));
		return true;
	}
}