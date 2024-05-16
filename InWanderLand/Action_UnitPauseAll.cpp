#include "Action_UnitPauseAll.h"
#include "LocalTimeEntityManager.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"

namespace application
{
    Action_UnitPauseAll::Action_UnitPauseAll(bool pause)
    {
        this->pause = pause;
    }

    CoroutineObject<void> Action_UnitPauseAll::DoAction()
    {
        // 모든 유닛들의 동작을 일시정지시킨다. 다만 플레이어의 경우 스킬은 쓸 수 있어야 한다.
        // usecase : 튜토리얼에서 스킬의 사용법을 알려주기 위해 잠깐 적들과 아군의 움직임을 멈추고 싶을 때
        if (pause)
        {
            //LocalTimeEntityManager::Instance().ReportTacticModeEngaged();
        }
        else
        {
            //LocalTimeEntityManager::Instance().ReportTacticModeEnded();
        }
        co_return;
    }

    void Action_UnitPauseAll::SetPause(bool pause)
    {
        this->pause = pause;
    }

    void Action_UnitPauseAll::ImGui_DrawDataPopup(Action_UnitPauseAll* data)
    {
        if (ImGui::MenuItem("SetPauseUnpause"))
        {
            editor::EditorLayer::SetInputControl(false);
            static bool pause = true;
            pause = data->pause;
            editor::imgui::ShowMessageBox("SetPauseUnpause", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::Checkbox("##Pause", &pause);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->SetPause(pause);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetPauseUnpause");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetPauseUnpause");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_UnitPauseAll::PreEncoding(json& data) const
    {
        data["pause"] = pause;
        return true;
    }

    bool Action_UnitPauseAll::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_UnitPauseAll::PreDecoding(const json& data)
    {
        pause = data["pause"];
        return true;
    }

    bool Action_UnitPauseAll::PostDecoding(const json& data)
    {
        return true;
    }
}