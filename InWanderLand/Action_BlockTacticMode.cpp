#include "Action_BlockTacticMode.h"
#include "LocalTimeEntityManager.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
	Action_BlockTacticMode::Action_BlockTacticMode(bool block)
	{
		this->block = block;
	}

	CoroutineObject<void> Action_BlockTacticMode::DoAction()
	{
		PlayerController::Instance().tacticActionPermitted = !block;
		co_return;
	}

	void Action_BlockTacticMode::SetBlock(bool block)
	{
		this->block = block;
	}

	void Action_BlockTacticMode::ImGui_DrawDataPopup(Action_BlockTacticMode* data)
	{
		if (ImGui::MenuItem("SetBlockUnblock"))
		{
			editor::EditorLayer::SetInputControl(false);
			static bool block = true;
			block = data->block;
			editor::imgui::ShowMessageBox("SetBlockUnblock(TacticMode)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::Checkbox("##Block", &block);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetBlock(block);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetBlockUnblock(TacticMode)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetBlockUnblock(TacticMode)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_BlockTacticMode::PreEncoding(json& data) const
	{
		data["block"] = block;
		return true;
	}

	bool Action_BlockTacticMode::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_BlockTacticMode::PreDecoding(const json& data)
	{
		block = data["block"];
		return true;
	}

	bool Action_BlockTacticMode::PostDecoding(const json& data)
	{
		return true;
	}
}

