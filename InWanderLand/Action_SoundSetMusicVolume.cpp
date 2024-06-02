#include "Action_SoundSetMusicVolume.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"

namespace application
{
    CoroutineObject<void> Action_SoundSetMusicVolume::DoAction()
    {
        float startVolume = SoundSystem::GetMusicVolume();
        if (fadeTime != 0)
        {
            float normalizedT = 0;
            float localTimer = 0;
            while (normalizedT < 1)
            {
                localTimer += yunutyEngine::Time::GetDeltaTimeUnscaled();
                normalizedT = localTimer / fadeTime;
                if (normalizedT > 1)
                {
                    normalizedT = 1;
                }
                SoundSystem::SetMusicVolume(math::LerpF(startVolume, volume, normalizedT));
                co_await std::suspend_always();
            }
        }
        SoundSystem::SetMusicVolume(volume);

        co_return;
    }

    void Action_SoundSetMusicVolume::SetFadeTime(float fadeTime)
    {
        this->fadeTime = fadeTime;
    }

    void Action_SoundSetMusicVolume::SetVolume(float volume)
    {
        this->volume = volume;
    }

    void Action_SoundSetMusicVolume::ImGui_DrawDataPopup(Action_SoundSetMusicVolume* data)
    {
        if (ImGui::MenuItem("Set Music Volume"))
        {
            editor::EditorLayer::SetInputControl(false);
            static float fadeTime = 0;
            static float volume = 0;
            fadeTime = data->fadeTime;
            volume = data->volume;
            editor::imgui::ShowMessageBox("Set Music Volume", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::DragFloat("Music Fade Time", &fadeTime);
                    ImGui::DragFloat("Music Volume", &volume);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->SetFadeTime(fadeTime);
                        data->SetVolume(volume);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("Set Music Volume");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("Set Music Volume");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_SoundSetMusicVolume::PreEncoding(json& data) const
    {
        data["fadeTime"] = fadeTime;
        data["volume"] = volume;
        return true;
    }

    bool Action_SoundSetMusicVolume::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_SoundSetMusicVolume::PreDecoding(const json& data)
    {
        fadeTime = data["fadeTime"];
        volume = data["volume"];
        return true;
    }

    bool Action_SoundSetMusicVolume::PostDecoding(const json& data)
    {
        return true;
    }
}