#include "Action_PauseWaves.h"
#include "LocalTimeEntityManager.h"
#include "InWanderLand.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"

namespace application
{
    Action_PauseWaves::Action_PauseWaves(bool pause)
    {
        this->pause = pause;
    }

    CoroutineObject<void> Action_PauseWaves::DoAction()
    {
        if (pause)
        {
            for (auto each : PlaytimeWave::GetInstances())
            {
                each->StopWaveElapsedTime();
            }
        }
        else
        {
            for (auto each : PlaytimeWave::GetInstances())
            {
                each->ResumeWaveElapsedTime();
            }
        }
        co_return;
    }

    void Action_PauseWaves::SetPause(bool pause)
    {
        this->pause = pause;
    }

    void Action_PauseWaves::ImGui_DrawDataPopup(Action_PauseWaves* data)
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

    bool Action_PauseWaves::PreEncoding(json& data) const
    {
        data["pause"] = pause;
        return true;
    }

    bool Action_PauseWaves::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_PauseWaves::PreDecoding(const json& data)
    {
        pause = data["pause"];
        return true;
    }

    bool Action_PauseWaves::PostDecoding(const json& data)
    {
        return true;
    }
}