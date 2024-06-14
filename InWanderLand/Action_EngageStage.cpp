#include "Action_EngageStage.h"
#include "LocalTimeEntityManager.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "InWanderLand.h"

namespace application
{
    Action_EngageStage::Action_EngageStage(int stageIndex)
    {
        this->index = stageIndex;
    }

    CoroutineObject<void> Action_EngageStage::DoAction()
    {
        for (auto each : editor::palette::OrnamentEditorInstance::GetInstances())
        {
            if (each->GetOrnamentData()->pod.stage == index)
                each->GetGameObject()->SetSelfActive(true);
            else
                each->GetGameObject()->SetSelfActive(false);
        }
        if (index == 1)
        {
            wanderUtils::ChangeStageToOne();
        }
        if (index == 2)
        {
            wanderUtils::ChangeStageToTwo();
        }
        co_return;
    }

    void Action_EngageStage::SetStageIndex(int index)
    {
        this->index = index;
    }

    void Action_EngageStage::ImGui_DrawDataPopup(Action_EngageStage* data)
    {
        if (ImGui::MenuItem("SetStageIndex"))
        {
            editor::EditorLayer::SetInputControl(false);
            static int index = 0;
            index = data->index;
            editor::imgui::ShowMessageBox("SetStageIndex", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::InputInt("##EngageStageIndex", &index);

                    ImGui::Separator();
                    index = clamp(index, 1, 2);

                    if (ImGui::Button("OK"))
                    {
                        data->SetStageIndex(index);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetStageIndex");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetStageIndex");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_EngageStage::PreEncoding(json& data) const
    {
        data["index"] = index;
        return true;
    }

    bool Action_EngageStage::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_EngageStage::PreDecoding(const json& data)
    {
        index = data["index"];
        return true;
    }

    bool Action_EngageStage::PostDecoding(const json& data)
    {
        return true;
    }
}