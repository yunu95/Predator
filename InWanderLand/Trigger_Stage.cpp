#include "Trigger_Stage.h"

#include "Application.h"
#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
	void Trigger_StageStart::LinkCallback()
	{
        wanderUtils::PushStageCallback((wanderUtils::Stage)(index - 1),
            [=]() { PullTrigger(); });
	}

    void Trigger_StageStart::SetStageIndex(int index)
    {
        this->index = index;
    }

	void Trigger_StageStart::ImGui_DrawDataPopup(Trigger_StageStart* data)
	{
        if (ImGui::MenuItem("SetStageIndex"))
        {
            editor::EditorLayer::SetInputControl(false);
            static int index = 0;
            index = data->index;
            editor::imgui::ShowMessageBox("SetStageIndex(Start Trigger)", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::InputInt("##StartStageIndex", &index);

                    ImGui::Separator();
                    index = clamp(index, 1, 2);

                    if (ImGui::Button("OK"))
                    {
                        data->SetStageIndex(index);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetStageIndex(Start Trigger)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetStageIndex(Start Trigger)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
	}

	bool Trigger_StageStart::PreEncoding(json& data) const
	{
        data["index"] = index;
		return true;
	}

	bool Trigger_StageStart::PostEncoding(json& data) const
	{
		return true;
	}

	bool Trigger_StageStart::PreDecoding(const json& data)
	{
        index = data["index"];
		return true;
	}

	bool Trigger_StageStart::PostDecoding(const json& data)
	{
		return true;
	}
}
