#include "Action_SetCamBidirectionalPath.h"

#include "InWanderLand.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorPopupManager.h"
#include "PlayerController.h"

namespace application
{
    CoroutineObject<void> Action_SetCamBidirectionalPath::DoAction()
    {
        co_return;
    }

    void Action_SetCamBidirectionalPath::ImGui_DrawDataPopup(Action_SetCamBidirectionalPath* data)
    {
        if (ImGui::MenuItem("Set Cam Path"))
        {
            editor::EditorLayer::SetInputControl(false);
            static float pathX = 1;
            static float pathZ = 1;
            static bool applyZoomFactor = false;
            static float zoomFactor = 10;
            pathX = data->pathX;
            pathZ = data->pathZ;
            applyZoomFactor = data->applyZoomFactor;
            zoomFactor = data->zoomFactor;
            editor::imgui::ShowMessageBox("Set Cam Path", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::DragFloat("path X", &pathX);
                    ImGui::DragFloat("path Z", &pathZ);
                    ImGui::Checkbox("apply Zoom Factor", &applyZoomFactor);
                    ImGui::DragFloat("Zoom Factor", &zoomFactor);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->pathX = pathX;
                        data->pathZ = pathZ;
                        data->applyZoomFactor = applyZoomFactor;
                        data->zoomFactor = zoomFactor;
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("Set Cam Path");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("Set Cam Path");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_SetCamBidirectionalPath::PreEncoding(json& data) const
    {
        data["pathX"] = pathX;
        data["pathZ"] = pathZ;
        data["applyZoomFactor"] = applyZoomFactor;
        data["zoomFactor"] = zoomFactor;
        return true;
    }

    bool Action_SetCamBidirectionalPath::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_SetCamBidirectionalPath::PreDecoding(const json& data)
    {
        pathX = data["pathX"];
        pathZ = data["pathZ"];
        applyZoomFactor = data["applyZoomFactor"];
        zoomFactor = data["zoomFactor"];
        return true;
    }

    bool Action_SetCamBidirectionalPath::PostDecoding(const json& data)
    {
        return true;
    }
}