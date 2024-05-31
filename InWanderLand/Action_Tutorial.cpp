#include "Action_Tutorial.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"

#include "TutorialManager.h"

namespace application
{
    CoroutineObject<void> Action_TutorialModeChange::DoAction()
    {
        //TutorialManager::Instance().SetTutorialMode(tutorialMode);
        co_return;
    }

    void Action_TutorialModeChange::SetTutorialMode(bool tutorialMode)
    {
        this->tutorialMode = tutorialMode;
    }

    void Action_TutorialModeChange::ImGui_DrawDataPopup(Action_TutorialModeChange* data)
    {
        if (ImGui::MenuItem("SetTutorialMode"))
        {
            editor::EditorLayer::SetInputControl(false);
            static bool tutorialMode = false;
            tutorialMode = data->tutorialMode;
            editor::imgui::ShowMessageBox("SetTutorialMode", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::Checkbox("Tutorial Mode", &tutorialMode);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->SetTutorialMode(tutorialMode);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTutorialMode");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTutorialMode");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_TutorialModeChange::PreEncoding(json& data) const
    {
        data["tutorialMode"] = tutorialMode;
        return true;
    }

    bool Action_TutorialModeChange::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_TutorialModeChange::PreDecoding(const json& data)
    {
        tutorialMode = data["tutorialMode"];
        return true;
    }

    bool Action_TutorialModeChange::PostDecoding(const json& data)
    {
        return true;
    }
}