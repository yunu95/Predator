#include "Action_SetZoomFactor.h"
#include "InWanderLand.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"

namespace application
{
    CoroutineObject<void> Action_SetZoomFactor::DoAction()
    {
        if (lerpTime <= 0)
        {
            PlayerController::Instance().SetZoomFactor(zoomFactor);
            co_return;
        }
        coroutine::ForSeconds forSeconds{ lerpTime };
        float zoomStart = PlayerController::Instance().GetZoomFactor();
        while (forSeconds.Tick())
        {
            PlayerController::Instance().SetZoomFactor(math::LerpF(zoomStart, zoomFactor, forSeconds.ElapsedNormalized()));
            co_await std::suspend_always{};
        }
        co_return;
    }

    void Action_SetZoomFactor::ImGui_DrawDataPopup(Action_SetZoomFactor* data)
    {
        if (ImGui::MenuItem("Set Zoom Factor"))
        {
            editor::EditorLayer::SetInputControl(false);
            static float lerpTime = 0;
            static float zoomFactor = 0;
            lerpTime = data->lerpTime;
            zoomFactor = data->zoomFactor;
            editor::imgui::ShowMessageBox("Set Zoom Factor", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::DragFloat("Zoom Lerp Time", &lerpTime);
                    ImGui::DragFloat("Zoom Factor", &zoomFactor);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->lerpTime = lerpTime;
                        data->zoomFactor = zoomFactor;
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("Set Zoom Factor");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("Set Zoom Factor");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_SetZoomFactor::PreEncoding(json& data) const
    {
        data["lerpTime"] = lerpTime;
        data["zoomFactor"] = zoomFactor;
        return true;
    }

    bool Action_SetZoomFactor::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_SetZoomFactor::PreDecoding(const json& data)
    {
        lerpTime = data["lerpTime"];
        zoomFactor = data["zoomFactor"];
        return true;
    }

    bool Action_SetZoomFactor::PostDecoding(const json& data)
    {
        return true;
    }
}