#include "Action_RequestPeace.h"

#include "InWanderLand.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorPopupManager.h"
#include "PlayerController.h"

namespace application
{
	CoroutineObject<void> Action_RequestPeace::DoAction()
	{
		PlayerController::Instance().RequestStateFromAction(PlayerController::State::Peace, requestOn);
		co_return;
	}

	void Action_RequestPeace::SetRequest(bool requestOn)
	{
		this->requestOn = requestOn;
	}

	void Action_RequestPeace::ImGui_DrawDataPopup(Action_RequestPeace* data)
	{
		if (ImGui::MenuItem("SetRequest(Peace)"))
		{
			editor::EditorLayer::SetInputControl(false);
			static bool requestOn = true;
			requestOn = data->requestOn;
			editor::imgui::ShowMessageBox("SetRequest(Peace)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::Checkbox("Request##Peace", &requestOn);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetRequest(requestOn);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRequest(Peace)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRequest(Peace)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_RequestPeace::PreEncoding(json& data) const
	{
		data["requestOn"] = requestOn;
		return true;
	}

	bool Action_RequestPeace::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_RequestPeace::PreDecoding(const json& data)
	{
		requestOn = data["requestOn"];
		return true;
	}

	bool Action_RequestPeace::PostDecoding(const json& data)
	{
		return true;
	}
}