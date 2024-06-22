#include "Condition_PlayerState.h"

#include "InWanderLand.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorPopupManager.h"
#include "PlayerController.h"
#include "Module_ScriptEditor.h"

namespace application
{
	void Condition_PlayerState::SetState(int state)
	{
		this->state = state;
	}

	void Condition_PlayerState::ImGui_DrawDataPopup(Condition_PlayerState* data)
	{
		if (ImGui::MenuItem("Condition State"))
		{
			editor::EditorLayer::SetInputControl(false);
			static int state = PlayerController::State::Enum::None;
			state = data->state;
			editor::imgui::ShowMessageBox("Set Condition State", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);

					static std::unordered_map<int, std::string> stateMap = { {PlayerController::State::Enum::None, "None"}, {PlayerController::State::Enum::Battle, "Battle"}, {PlayerController::State::Enum::Peace, "Peace"}, {PlayerController::State::Enum::Tactic, "Tactic"}, {PlayerController::State::Enum::Cinematic, "Cinematic"} };
					std::string current = stateMap[state];
					if (ImGui::BeginCombo("##ConditionStateCombo", current.c_str()))
					{
						for (int i = 0; i < stateMap.size(); i++)
						{
							const bool is_selected = (current == stateMap[i]);
							if (ImGui::Selectable(stateMap[i].c_str(), is_selected))
							{
								current = stateMap[i];
								state = i;
							}

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetState(state);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRequestState");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRequestState");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Condition_PlayerState::IsConditionMet()
	{
		return PlayerController::Instance().GetState() == state;
	}

	bool Condition_PlayerState::PreEncoding(json& data) const
	{
		data["state"] = state;
		return true;
	}

	bool Condition_PlayerState::PostEncoding(json& data) const
	{
		return true;
	}

	bool Condition_PlayerState::PreDecoding(const json& data)
	{
		state = data["state"];
		return true;
	}

	bool Condition_PlayerState::PostDecoding(const json& data)
	{
		return true;
	}
}