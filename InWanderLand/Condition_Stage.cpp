#include "Condition_Stage.h"

#include "InWanderLand.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorPopupManager.h"
#include "PlayerController.h"
#include "Module_ScriptEditor.h"

namespace application
{
	void Condition_Stage::SetStageIndex(int index)
	{
		this->index = index;
	}

	void Condition_Stage::ImGui_DrawDataPopup(Condition_Stage* data)
	{
		if (ImGui::MenuItem("SetStageIndex"))
		{
			editor::EditorLayer::SetInputControl(false);
			static int index = 0;
			index = data->index;
			editor::imgui::ShowMessageBox("SetStageIndex(Condition)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::InputInt("##ConditionStageIndex", &index);

					ImGui::Separator();
					index = clamp(index, 1, 2);

					if (ImGui::Button("OK"))
					{
						data->SetStageIndex(index);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetStageIndex(Condition)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetStageIndex(Condition)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Condition_Stage::IsConditionMet()
	{
		return wanderUtils::GetCurrentStage() == (wanderUtils::Stage)(index - 1);
	}

	bool Condition_Stage::PreEncoding(json& data) const
	{
		data["index"] = index;
		return true;
	}

	bool Condition_Stage::PostEncoding(json& data) const
	{
		return true;
	}

	bool Condition_Stage::PreDecoding(const json& data)
	{
		index = data["index"];
		return true;
	}

	bool Condition_Stage::PostDecoding(const json& data)
	{
		return true;
	}
}