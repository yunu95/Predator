#include "Action_RequestBattle.h"

#include "InWanderLand.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorPopupManager.h"
#include "PlayerController.h"

namespace application
{
	CoroutineObject<void> Action_RequestBattle::DoAction()
	{
		PlayerController::Instance().RequestStateFromAction(PlayerController::State::Battle, requestOn);
		co_return;
	}

	void Action_RequestBattle::SetRequest(bool requestOn)
	{
		this->requestOn = requestOn;
	}

	void Action_RequestBattle::ImGui_DrawDataPopup(Action_RequestBattle* data)
	{
		if (ImGui::MenuItem("SetRequest(Battle)"))
		{
			editor::EditorLayer::SetInputControl(false);
			static bool requestOn = true;
			requestOn = data->requestOn;
			editor::imgui::ShowMessageBox("SetRequest(Battle)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::Checkbox("Request##Battle", &requestOn);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetRequest(requestOn);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRequest(Battle)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRequest(Battle)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_RequestBattle::PreEncoding(json& data) const
	{
		data["requestOn"] = requestOn;
		return true;
	}

	bool Action_RequestBattle::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_RequestBattle::PreDecoding(const json& data)
	{
		requestOn = data["requestOn"];
		return true;
	}

	bool Action_RequestBattle::PostDecoding(const json& data)
	{
		return true;
	}
}