#include "Action_SoundPlay.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "UIManager.h"
#include "UIElement.h"

#include "CinematicManager.h"

#include "Action_WaitForSeconds.h"

namespace application
{
    CoroutineObject<void> Action_SoundPlay::DoAction()
    {
        SoundSystem::PlaySoundfile(soundPath);
        co_return;
    }

    void Action_SoundPlay::ImGui_DrawDataPopup(Action_SoundPlay* data)
    {
        if (ImGui::MenuItem("SetSoundPath"))
        {
            editor::EditorLayer::SetInputControl(false);
            static string soundPath = "";
            soundPath = data->soundPath;
            editor::imgui::ShowMessageBox("SetSoundPath", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    if (ImGui::BeginCombo("Sound Path", soundPath.c_str()))
                    {
                        vector<string> soundList;
                        transform(SoundSystem::GetLoadedSoundsList().begin(), SoundSystem::GetLoadedSoundsList().end(), back_inserter(soundList), [](const string& sound) { return sound; });
                        std::sort(soundList.begin(), soundList.end());

                        for (const auto& key : soundList)
                        {
                            bool isSelected = (data->soundPath == key);
                            if (ImGui::Selectable(key.c_str(), isSelected))
                            {
                                soundPath = key;
                            }
                            if (isSelected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->soundPath = soundPath;
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetSoundPath");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetSoundPath");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_SoundPlay::PreEncoding(json& data) const
    {
        data["soundPath"] = soundPath;
        return true;
    }

    bool Action_SoundPlay::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_SoundPlay::PreDecoding(const json& data)
    {
        soundPath = data["soundPath"];
        return true;
    }

    bool Action_SoundPlay::PostDecoding(const json& data)
    {
        return true;
    }
}