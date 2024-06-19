#include "Action_RequestState.h"

#include "InWanderLand.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorPopupManager.h"
#include "PlayerController.h"

namespace application
{
	CoroutineObject<void> Action_RequestState::DoAction()
	{
		PlayerController::Instance().RequestStateFromAction((PlayerController::State::Enum)request);
		co_return;
	}

	void Action_RequestState::SetRequest(int request)
	{
		this->request = request;
	}

	void Action_RequestState::ImGui_DrawDataPopup(Action_RequestState* data)
	{
		if (ImGui::MenuItem("SetRequestState"))
		{
			editor::EditorLayer::SetInputControl(false);
			static int request = PlayerController::State::Enum::None;
			request = data->request;
			editor::imgui::ShowMessageBox("SetRequestState", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);

					static std::unordered_map<int, std::string> stateMap = { {PlayerController::State::Enum::None, "None"}, {PlayerController::State::Enum::Battle, "Battle"}, {PlayerController::State::Enum::Peace, "Peace"}};
					std::string current = stateMap[request];
					if (ImGui::BeginCombo("##requestCombo", current.c_str()))
					{
						for (int i = 0; i < stateMap.size(); i++)
						{
							const bool is_selected = (current == stateMap[i]);
							if (ImGui::Selectable(stateMap[i].c_str(), is_selected))
							{
								current = stateMap[i];
								request = i;
							}

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetRequest(request);
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

	bool Action_RequestState::PreEncoding(json& data) const
	{
		data["request"] = request;
		return true;
	}

	bool Action_RequestState::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_RequestState::PreDecoding(const json& data)
	{
		request = data["request"];
		return true;
	}

	bool Action_RequestState::PostDecoding(const json& data)
	{
		return true;
	}
}