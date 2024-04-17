#include "Action_Cinematic.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"

#include "CinematicManager.h"

namespace application
{
	CoroutineObject<void> Action_CinematicModeChange::DoAction()
	{
		CinematicManager::Instance().SetCinematicMode(cinematicMode);
		co_return;
	}

	void Action_CinematicModeChange::SetCinematicMode(bool cinematicMode)
	{
		this->cinematicMode = cinematicMode;
	}

	void Action_CinematicModeChange::ImGui_DrawDataPopup(Action_CinematicModeChange* data)
	{
		if (ImGui::MenuItem("SetCinematicMode"))
		{
			editor::EditorLayer::SetInputControl(false);
			static bool cinematicMode = false;
			cinematicMode = data->cinematicMode;
			editor::imgui::ShowMessageBox("SetCinematicMode", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::Checkbox("Cinematic Mode", &cinematicMode);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetCinematicMode(cinematicMode);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetCinematicMode");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetCinematicMode");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_CinematicModeChange::PreEncoding(json& data) const
	{
		data["cinematicMode"] = cinematicMode;
		return true;
	}

	bool Action_CinematicModeChange::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_CinematicModeChange::PreDecoding(const json& data)
	{
		cinematicMode = data["cinematicMode"];
		return true;
	}

	bool Action_CinematicModeChange::PostDecoding(const json& data)
	{
		return true;
	}
}
