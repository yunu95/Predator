#include "Action_SoundPlayMusic.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "UIManager.h"
#include "UIElement.h"

#include "CinematicManager.h"

#include "Action_WaitForSeconds.h"

namespace application
{
    CoroutineObject<void> Action_SoundPlayMusic::DoAction()
    {
        SoundSystem::PlayMusic(soundPath);
        co_return;
    }

    bool Action_SoundPlayMusic::IsValid()
    {
        return !soundPath.empty();
    }

    void Action_SoundPlayMusic::ImGui_DrawDataPopup(Action_SoundPlayMusic* data)
    {
        if (ImGui::MenuItem("SetMusicPath"))
        {
            editor::EditorLayer::SetInputControl(false);
            static string soundPath = "";
            soundPath = data->soundPath;
            editor::imgui::ShowMessageBox("SetMusicPath", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    if (ImGui::BeginCombo("Music Path", soundPath.c_str()))
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
                        editor::imgui::CloseMessageBox("SetMusicPath");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetMusicPath");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_SoundPlayMusic::PreEncoding(json& data) const
    {
        data["soundPath"] = soundPath;
        return true;
    }

    bool Action_SoundPlayMusic::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_SoundPlayMusic::PreDecoding(const json& data)
    {
        soundPath = data["soundPath"];
        return true;
    }

    bool Action_SoundPlayMusic::PostDecoding(const json& data)
    {
        return true;
    }
}