#include "Trigger_Shortcut.h"

#include "GamePlayTimer.h"
#include "EditorLayer.h"

namespace application
{
    void Trigger_Shortcut::LinkCallback()
    {
        GamePlayTimer::Instance().repeatCallback[repeatTime].push_back(
            [this]()
            {
                PullTrigger();
            }
        );
    }

    void Trigger_Shortcut::SetRepeatTime(float time)
    {
        repeatTime = time;
    }

    void Trigger_Shortcut::ImGui_DrawDataPopup(Trigger_Shortcut* data)
    {
        if (ImGui::MenuItem("SetRepeatTime"))
        {
            editor::EditorLayer::SetInputControl(false);
            static float repeatTime = 0;
            repeatTime = data->repeatTime;
            editor::imgui::ShowMessageBox("SetRepeatTime", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat("##repeatTime", &repeatTime);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->SetRepeatTime(repeatTime);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetRepeatTime");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetRepeatTime");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Trigger_Shortcut::PreEncoding(json& data) const
    {
        data["repeatTime"] = repeatTime;
        return true;
    }

    bool Trigger_Shortcut::PostEncoding(json& data) const
    {
        return true;
    }

    bool Trigger_Shortcut::PreDecoding(const json& data)
    {
        repeatTime = data["repeatTime"];
        return true;
    }

    bool Trigger_Shortcut::PostDecoding(const json& data)
    {
        return true;
    }
}