#include "Action_SetTimeScale.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"

namespace application
{
    Action_SetTimeScale::Action_SetTimeScale(float timescale)
        : timeScale(timescale)
    {

    }

    CoroutineObject<void> Action_SetTimeScale::DoAction()
    {
        Time::SetTimeScale(timeScale);
        co_return;
    }

    void Action_SetTimeScale::SetTimeScale(float timeScale)
    {
        this->timeScale = timeScale;
    }

    void Action_SetTimeScale::ImGui_DrawDataPopup(Action_SetTimeScale* data)
    {
        if (ImGui::MenuItem("SetTimeScale(scale)"))
        {
            editor::EditorLayer::SetInputControl(false);
            static float timeScale = 0;
            timeScale = data->timeScale;
            editor::imgui::ShowMessageBox("SetTimeScale(scale)", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat("##TimeScale", &timeScale);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->SetTimeScale(timeScale);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTimeScale(scale)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTimeScale(scale)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_SetTimeScale::PreEncoding(json& data) const
    {
        data["timeScale"] = timeScale;
        return true;
    }

    bool Action_SetTimeScale::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_SetTimeScale::PreDecoding(const json& data)
    {
        timeScale = data["timeScale"];
        return true;
    }

    bool Action_SetTimeScale::PostDecoding(const json& data)
    {
        return true;
    }
}