#include "Action_Cinematic.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "UIManager.h"

#include "CinematicManager.h"

namespace application
{
    CoroutineObject<void> Action_CinematicModeChange::DoAction()
    {
        CinematicManager::Instance().SetCinematicMode(cinematicMode);
        co_return;
    }

    void Action_CinematicModeChange::SetCinematicMode(bool cinematicMode)
    {
        this->cinematicMode = cinematicMode;
    }

    void Action_CinematicModeChange::ImGui_DrawDataPopup(Action_CinematicModeChange* data)
    {
        if (ImGui::MenuItem("SetCinematicMode"))
        {
            editor::EditorLayer::SetInputControl(false);
            static bool cinematicMode = false;
            cinematicMode = data->cinematicMode;
            editor::imgui::ShowMessageBox("SetCinematicMode", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::Checkbox("Cinematic Mode", &cinematicMode);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->SetCinematicMode(cinematicMode);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetCinematicMode");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetCinematicMode");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_CinematicModeChange::PreEncoding(json& data) const
    {
        data["cinematicMode"] = cinematicMode;
        return true;
    }

    bool Action_CinematicModeChange::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_CinematicModeChange::PreDecoding(const json& data)
    {
        cinematicMode = data["cinematicMode"];
        return true;
    }

    bool Action_CinematicModeChange::PostDecoding(const json& data)
    {
        return true;
    }

    CoroutineObject<void> Action_CinematicFadeIn::DoAction()
    {
        /// FadeIn
        co_return;
    }

    void Action_CinematicFadeIn::SetFadeTime(float fadeTime)
    {
        this->fadeTime = fadeTime;
    }

    void Action_CinematicFadeIn::ImGui_DrawDataPopup(Action_CinematicFadeIn* data)
    {
        if (ImGui::MenuItem("SetFadeInTime"))
        {
            editor::EditorLayer::SetInputControl(false);
            static float fadeTime = 0;
            fadeTime = data->fadeTime;
            editor::imgui::ShowMessageBox("SetFadeInTime", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat("##FadeInTime", &fadeTime);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->SetFadeTime(fadeTime);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetFadeInTime");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetFadeInTime");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_CinematicFadeIn::PreEncoding(json& data) const
    {
        data["fadeTime"] = fadeTime;
        return true;
    }

    bool Action_CinematicFadeIn::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_CinematicFadeIn::PreDecoding(const json& data)
    {
        fadeTime = data["fadeTime"];
        return true;
    }

    bool Action_CinematicFadeIn::PostDecoding(const json& data)
    {
        return true;
    }

    CoroutineObject<void> Action_CinematicFadeOut::DoAction()
    {
        /// FadeOut
        co_return;
    }

    void Action_CinematicFadeOut::SetFadeTime(float fadeTime)
    {
        this->fadeTime = fadeTime;
    }

    void Action_CinematicFadeOut::ImGui_DrawDataPopup(Action_CinematicFadeOut* data)
    {
        if (ImGui::MenuItem("SetFadeOutTime"))
        {
            editor::EditorLayer::SetInputControl(false);
            static float fadeTime = 0;
            fadeTime = data->fadeTime;
            editor::imgui::ShowMessageBox("SetFadeOutTime", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat("##FadeOutTime", &fadeTime);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->SetFadeTime(fadeTime);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetFadeOutTime");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetFadeOutTime");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_CinematicFadeOut::PreEncoding(json& data) const
    {
        data["fadeTime"] = fadeTime;
        return true;
    }

    bool Action_CinematicFadeOut::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_CinematicFadeOut::PreDecoding(const json& data)
    {
        fadeTime = data["fadeTime"];
        return true;
    }

    bool Action_CinematicFadeOut::PostDecoding(const json& data)
    {
        return true;
    }
}
