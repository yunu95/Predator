#include "Action_Camera.h"

#include "InWanderLand.h"
#include "CameraData.h"

#include "Application.h"
#include "EditorLayer.h"
#include "RTSCam.h"
#include "Panel_Palette.h"
#include "CameraPalette.h"

#include "EditorPopupManager.h"

#include "PlayerController.h"

namespace application
{
    std::unordered_map<editor::CameraData*, GameObject*> Action_SetTacticCamera::camObjMap;
    Action_CameraChangeView::~Action_CameraChangeView()
    {
        if (targetCam)
        {
            targetCam->RemoveObserver(this);
        }
    }

    CoroutineObject<void> Action_CameraChangeView::DoAction()
    {
        RTSCam* mainCam = static_cast<RTSCam*>(yunutyEngine::graphics::Camera::GetMainCamera());
        mainCam->SetUpdateability(false);
        auto ts = mainCam->GetTransform();
        auto startPos = ts->GetWorldPosition();
        auto startRot = ts->GetWorldRotation();
        auto startScale = ts->GetWorldScale();
        auto& camGI = mainCam->GetGI();
        float startFov = camGI.GetVerticalFOV();
        float startCameraNear = camGI.GetNear();
        float startCameraFar = camGI.GetFar();
        float startWidth = 0;
        float startHeight = 0;
        camGI.GetResolution(&startWidth, &startHeight);

        Vector3d endPos = { targetCam->pod.position.x, targetCam->pod.position.y, targetCam->pod.position.z };
        Quaternion endRot = { targetCam->pod.rotation.w, targetCam->pod.rotation.x, targetCam->pod.rotation.y, targetCam->pod.rotation.z };
        Vector3d endScale = { targetCam->pod.scale.x, targetCam->pod.scale.y, targetCam->pod.scale.z };

        float endFov = targetCam->pod.vertical_FOV;
        float endCameraNear = targetCam->pod.dis_Near;
        float endCameraFar = targetCam->pod.dis_Far;
        float endWidth = targetCam->pod.res_Width;
        float endHeight = targetCam->pod.res_Height;

        double timer = 0;
        float factor = 0;

        if (lerpTime == 0)
        {
            ts->SetWorldPosition(endPos);
            ts->SetWorldRotation(endRot);
            ts->SetWorldScale(endScale);
            camGI.SetVerticalFOV(endFov);
            camGI.SetNear(endCameraNear);
            camGI.SetFar(endCameraFar);
            camGI.SetResolution(endWidth, endHeight);
            co_return;
        }

        for (double timer = 0; timer < lerpTime;)
        {
            factor = timer / lerpTime;
            ts->SetWorldPosition(Vector3d::Lerp(startPos, endPos, factor));
            ts->SetWorldRotation(Quaternion::Lerp(startRot, endRot, factor));
            ts->SetWorldScale(Vector3d::Lerp(startScale, endScale, factor));
            camGI.SetVerticalFOV(math::LerpF(startFov, endFov, factor));
            camGI.SetNear(math::LerpF(startCameraNear, endCameraNear, factor));
            camGI.SetFar(math::LerpF(startCameraFar, endCameraFar, factor));
            camGI.SetResolution(math::LerpF(startWidth, endWidth, factor), math::LerpF(startHeight, endHeight, factor));
            timer += Time::GetDeltaTimeUnscaled();
            co_await std::suspend_always();
        }
    }

    bool Action_CameraChangeView::IsValid()
    {
        return (targetCam != nullptr) ? true : false;
    }

    void Action_CameraChangeView::SetCamera(editor::CameraData* cam)
    {
        if (targetCam)
        {
            targetCam->RemoveObserver(this);
        }

        targetCam = cam;
        if (cam)
        {
            cam->RegisterObserver(this);
        }
    }

    void Action_CameraChangeView::SetLerpTime(float lerpTime)
    {
        this->lerpTime = lerpTime;
    }

    void Action_CameraChangeView::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
    {
        switch (event)
        {
        case application::ObservationEvent::Destroy:
        {
            if (targetCam == static_cast<editor::CameraData*>(target))
            {
                targetCam = nullptr;
            }
            break;
        }
        default:
            break;
        }
    }

    void Action_CameraChangeView::ImGui_DrawDataPopup(Action_CameraChangeView* data)
    {
        if (ImGui::MenuItem("SetTargetCamera"))
        {
            editor::EditorLayer::SetInputControl(false);
            editor::imgui::ShowMessageBox("SetTargetCamera", [data]()
                {
                    auto& pp = editor::PalettePanel::GetSingletonInstance();
                    auto& cp = editor::palette::CameraPalette::SingleInstance();

                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    if (data->targetCam)
                    {
                        ImGui::Text(data->targetCam->pod.name.c_str());
                        pp.ChangeTab("Camera");
                        cp.Reset();
                        cp.SelectCamera(data->targetCam);
                    }
                    else
                    {
                        ImGui::Text("------");
                    }

                    ImGui::Separator();

                    if (ImGui::Button("Edit"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTargetCamera");
                        editor::EditorLayer::SetInputControl(true);
                        editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_CameraChangeView>("SetTargetCamera", data);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTargetCamera");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }

        if (ImGui::MenuItem("SetLerpTime"))
        {
            editor::EditorLayer::SetInputControl(false);
            static float lerpTime = 0;
            lerpTime = data->lerpTime;
            editor::imgui::ShowMessageBox("SetLerpTime(ChangeView)", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat("##LerpTimeChangView", &lerpTime);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->SetLerpTime(lerpTime);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetLerpTime(ChangeView)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetLerpTime(ChangeView)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_CameraChangeView::PreEncoding(json& data) const
    {
        data["lerpTime"] = lerpTime;
        return true;
    }

    bool Action_CameraChangeView::PostEncoding(json& data) const
    {
        data["targetCam"] = targetCam ? UUID_To_String(targetCam->GetUUID()) : "nullptr";
        return true;
    }

    bool Action_CameraChangeView::PreDecoding(const json& data)
    {
        lerpTime = data["lerpTime"];
        return true;
    }

    bool Action_CameraChangeView::PostDecoding(const json& data)
    {
        SetCamera(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::CameraData*>(String_To_UUID(data["targetCam"])));
        return true;
    }

    float Action_CameraSaveView::savedFov = -1;
    float Action_CameraSaveView::savedCameraNear = -1;
    float Action_CameraSaveView::savedCameraFar = -1;
    float Action_CameraSaveView::savedWidth = -1;
    float Action_CameraSaveView::savedHeight = -1;
    yunuGI::Vector3 Action_CameraSaveView::savedPosition = yunuGI::Vector3();
    yunuGI::Quaternion Action_CameraSaveView::savedRotation = yunuGI::Quaternion();
    yunuGI::Vector3 Action_CameraSaveView::savedScale = yunuGI::Vector3();

    void Action_CameraSaveView::Clear()
    {
        savedFov = -1;
        savedCameraNear = -1;
        savedCameraFar = -1;
        savedWidth = -1;
        savedHeight = -1;
        savedPosition = yunuGI::Vector3();
        savedRotation = yunuGI::Quaternion();
        savedScale = yunuGI::Vector3();
    }

    CoroutineObject<void> Action_CameraSaveView::DoAction()
    {
        RTSCam* mainCam = static_cast<RTSCam*>(yunutyEngine::graphics::Camera::GetMainCamera());
        auto ts = mainCam->GetTransform();
        auto pos = ts->GetWorldPosition();
        savedPosition.x = pos.x;
        savedPosition.y = pos.y;
        savedPosition.z = pos.z;
        auto rot = ts->GetWorldRotation();
        savedRotation.w = rot.w;
        savedRotation.x = rot.x;
        savedRotation.y = rot.y;
        savedRotation.z = rot.z;
        auto scale = ts->GetWorldScale();
        savedScale.x = scale.x;
        savedScale.y = scale.y;
        savedScale.z = scale.z;
        auto& camGI = mainCam->GetGI();
        savedFov = camGI.GetVerticalFOV();
        savedCameraNear = camGI.GetNear();
        savedCameraFar = camGI.GetFar();
        savedWidth = 0;
        savedHeight = 0;
        camGI.GetResolution(&savedWidth, &savedHeight);
        co_return;
    }

    void Action_CameraSaveView::ImGui_DrawDataPopup(Action_CameraSaveView* data)
    {

    }

    bool Action_CameraSaveView::PreEncoding(json& data) const
    {
        return true;
    }

    bool Action_CameraSaveView::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_CameraSaveView::PreDecoding(const json& data)
    {
        return true;
    }

    bool Action_CameraSaveView::PostDecoding(const json& data)
    {
        return true;
    }

    CoroutineObject<void> Action_CameraLoadView::DoAction()
    {
        if (Action_CameraSaveView::savedFov == -1)
        {
            co_return;
        }

        RTSCam* mainCam = static_cast<RTSCam*>(yunutyEngine::graphics::Camera::GetMainCamera());
        mainCam->SetUpdateability(false);
        auto ts = mainCam->GetTransform();
        auto startPos = ts->GetWorldPosition();
        auto startRot = ts->GetWorldRotation();
        auto startScale = ts->GetWorldScale();
        auto& camGI = mainCam->GetGI();
        float startFov = camGI.GetVerticalFOV();
        float startCameraNear = camGI.GetNear();
        float startCameraFar = camGI.GetFar();
        float startWidth = 0;
        float startHeight = 0;
        camGI.GetResolution(&startWidth, &startHeight);

        Vector3d endPos = { Action_CameraSaveView::savedPosition.x, Action_CameraSaveView::savedPosition.y, Action_CameraSaveView::savedPosition.z };
        Quaternion endRot = { Action_CameraSaveView::savedRotation.w, Action_CameraSaveView::savedRotation.x, Action_CameraSaveView::savedRotation.y, Action_CameraSaveView::savedRotation.z };
        Vector3d endScale = { Action_CameraSaveView::savedScale.x, Action_CameraSaveView::savedScale.y, Action_CameraSaveView::savedScale.z };

        float endFov = Action_CameraSaveView::savedFov;
        float endCameraNear = Action_CameraSaveView::savedCameraNear;
        float endCameraFar = Action_CameraSaveView::savedCameraFar;
        float endWidth = Action_CameraSaveView::savedWidth;
        float endHeight = Action_CameraSaveView::savedHeight;

        double timer = 0;
        float factor = 0;

        if (lerpTime == 0)
        {
            ts->SetWorldPosition(endPos);
            ts->SetWorldRotation(endRot);
            ts->SetWorldScale(endScale);
            camGI.SetVerticalFOV(endFov);
            camGI.SetNear(endCameraNear);
            camGI.SetFar(endCameraFar);
            camGI.SetResolution(endWidth, endHeight);
            co_return;
        }

        for (double timer = 0; timer < lerpTime;)
        {
            factor = timer / lerpTime;
            ts->SetWorldPosition(Vector3d::Lerp(startPos, endPos, factor));
            ts->SetWorldRotation(Quaternion::Lerp(startRot, endRot, factor));
            ts->SetWorldScale(Vector3d::Lerp(startScale, endScale, factor));
            camGI.SetVerticalFOV(math::LerpF(startFov, endFov, factor));
            camGI.SetNear(math::LerpF(startCameraNear, endCameraNear, factor));
            camGI.SetFar(math::LerpF(startCameraFar, endCameraFar, factor));
            camGI.SetResolution(math::LerpF(startWidth, endWidth, factor), math::LerpF(startHeight, endHeight, factor));
            timer += Time::GetDeltaTimeUnscaled();
            co_await std::suspend_always();
        }
    }

    void Action_CameraLoadView::SetLerpTime(float lerpTime)
    {
        this->lerpTime = lerpTime;
    }

    void Action_CameraLoadView::ImGui_DrawDataPopup(Action_CameraLoadView* data)
    {
        if (ImGui::MenuItem("SetLerpTime"))
        {
            editor::EditorLayer::SetInputControl(false);
            static float lerpTime = 0;
            lerpTime = data->lerpTime;
            editor::imgui::ShowMessageBox("SetLerpTime(LoadView)", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat("##LerpTimeLoadView", &lerpTime);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->SetLerpTime(lerpTime);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetLerpTime(LoadView)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetLerpTime(LoadView)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_CameraLoadView::PreEncoding(json& data) const
    {
        data["lerpTime"] = lerpTime;
        return true;
    }

    bool Action_CameraLoadView::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_CameraLoadView::PreDecoding(const json& data)
    {
        lerpTime = data["lerpTime"];
        return true;
    }

    bool Action_CameraLoadView::PostDecoding(const json& data)
    {
        return true;
    }

    CoroutineObject<void> Action_CameraRevert::DoAction()
    {
        RTSCam* mainCam = static_cast<RTSCam*>(yunutyEngine::graphics::Camera::GetMainCamera());
        mainCam->SetUpdateability(false);
        auto ts = mainCam->GetTransform();
        auto startPos = ts->GetWorldPosition();
        auto startRot = ts->GetWorldRotation();
        auto startScale = ts->GetWorldScale();
        auto& camGI = mainCam->GetGI();
        float startFov = camGI.GetVerticalFOV();
        float startCameraNear = camGI.GetNear();
        float startCameraFar = camGI.GetFar();
        float startWidth = 0;
        float startHeight = 0;
        camGI.GetResolution(&startWidth, &startHeight);

        Vector3d endPos = mainCam->GetIdealPosition();
        Quaternion endRot = mainCam->GetInitRotation();
        Vector3d endScale = mainCam->GetInitScale();

        float endFov = mainCam->GetInitVerticalFOV();
        float endCameraNear = mainCam->GetInitNear();
        float endCameraFar = mainCam->GetInitFar();
        float endWidth = mainCam->GetInitWidth();
        float endHeight = mainCam->GetInitHeight();

        double timer = 0;
        float factor = 0;

        if (lerpTime == 0)
        {
            ts->SetWorldPosition(endPos);
            ts->SetWorldRotation(endRot);
            ts->SetWorldScale(endScale);
            camGI.SetVerticalFOV(endFov);
            camGI.SetNear(endCameraNear);
            camGI.SetFar(endCameraFar);
            camGI.SetResolution(endWidth, endHeight);
            mainCam->SetUpdateability(true);
            co_return;
        }

        for (double timer = 0; timer < lerpTime;)
        {
            factor = timer / lerpTime;
            endPos = mainCam->GetIdealPosition();
            ts->SetWorldPosition(Vector3d::Lerp(startPos, endPos, factor));
            ts->SetWorldRotation(Quaternion::Lerp(startRot, endRot, factor));
            ts->SetWorldScale(Vector3d::Lerp(startScale, endScale, factor));
            camGI.SetVerticalFOV(math::LerpF(startFov, endFov, factor));
            camGI.SetNear(math::LerpF(startCameraNear, endCameraNear, factor));
            camGI.SetFar(math::LerpF(startCameraFar, endCameraFar, factor));
            camGI.SetResolution(math::LerpF(startWidth, endWidth, factor), math::LerpF(startHeight, endHeight, factor));
            timer += Time::GetDeltaTimeUnscaled();
            co_await std::suspend_always();
        }

        mainCam->SetUpdateability(true);
    }

    void Action_CameraRevert::SetLerpTime(float lerpTime)
    {
        this->lerpTime = lerpTime;
    }

    void Action_CameraRevert::ImGui_DrawDataPopup(Action_CameraRevert* data)
    {
        if (ImGui::MenuItem("SetLerpTime"))
        {
            editor::EditorLayer::SetInputControl(false);
            static float lerpTime = 0;
            lerpTime = data->lerpTime;
            editor::imgui::ShowMessageBox("SetLerpTime(Revert)", [data]()
                {
                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat("##LerpTimeRevert", &lerpTime);

                    ImGui::Separator();

                    if (ImGui::Button("OK"))
                    {
                        data->SetLerpTime(lerpTime);
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetLerpTime(Revert)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetLerpTime(Revert)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_CameraRevert::PreEncoding(json& data) const
    {
        data["lerpTime"] = lerpTime;
        return true;
    }

    bool Action_CameraRevert::PostEncoding(json& data) const
    {
        return true;
    }

    bool Action_CameraRevert::PreDecoding(const json& data)
    {
        lerpTime = data["lerpTime"];
        return true;
    }

    bool Action_CameraRevert::PostDecoding(const json& data)
    {
        return true;
    }

    Action_SetTacticCamera::~Action_SetTacticCamera()
    {
        if (targetCam)
        {
            targetCam->RemoveObserver(this);
        }
    }

    CoroutineObject<void> Action_SetTacticCamera::DoAction()
    {
        if (camObjMap.contains(targetCam))
        {
            camObjMap[targetCam] = Scene::getCurrentScene()->AddGameObject();
            camObjMap[targetCam]->GetTransform()->SetWorldPosition({ targetCam->pod.position });
            camObjMap[targetCam]->GetTransform()->SetWorldRotation({ targetCam->pod.rotation.w, targetCam->pod.rotation.x, targetCam->pod.rotation.y, targetCam->pod.rotation.z });
        }
        PlayerController::Instance().SetTacticCamera(camObjMap[targetCam]);
        co_return;
    }

    bool Action_SetTacticCamera::IsValid()
    {
        return (targetCam != nullptr) ? true : false;
    }

    void Action_SetTacticCamera::SetCamera(editor::CameraData* cam)
    {
        if (targetCam)
        {
            targetCam->RemoveObserver(this);
        }

        targetCam = cam;
        if (cam)
        {
            cam->RegisterObserver(this);
        }
    }

    void Action_SetTacticCamera::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
    {
        switch (event)
        {
        case application::ObservationEvent::Destroy:
        {
            if (targetCam == static_cast<editor::CameraData*>(target))
            {
                targetCam = nullptr;
            }
            break;
        }
        default:
            break;
        }
    }

    void Action_SetTacticCamera::ImGui_DrawDataPopup(Action_SetTacticCamera* data)
    {
        if (ImGui::MenuItem("SetTargetCamera"))
        {
            editor::EditorLayer::SetInputControl(false);
            editor::imgui::ShowMessageBox("SetTargetCamera(Tactic)", [data]()
                {
                    auto& pp = editor::PalettePanel::GetSingletonInstance();
                    auto& cp = editor::palette::CameraPalette::SingleInstance();

                    editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(-1);
                    if (data->targetCam)
                    {
                        ImGui::Text(data->targetCam->pod.name.c_str());
                        pp.ChangeTab("Camera");
                        cp.Reset();
                        cp.SelectCamera(data->targetCam);
                    }
                    else
                    {
                        ImGui::Text("------");
                    }

                    ImGui::Separator();

                    if (ImGui::Button("Edit"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTargetCamera(Tactic)");
                        editor::EditorLayer::SetInputControl(true);
                        editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Action_SetTacticCamera>("SetTargetCamera(Tactic)", data);
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                        editor::imgui::CloseMessageBox("SetTargetCamera(Tactic)");
                        editor::EditorLayer::SetInputControl(true);
                    }
                }, 300);
        }
    }

    bool Action_SetTacticCamera::PreEncoding(json& data) const
    {
        return true;
    }

    bool Action_SetTacticCamera::PostEncoding(json& data) const
    {
        data["targetCam"] = targetCam ? UUID_To_String(targetCam->GetUUID()) : "nullptr";
        return true;
    }

    bool Action_SetTacticCamera::PreDecoding(const json& data)
    {
        return true;
    }

    bool Action_SetTacticCamera::PostDecoding(const json& data)
    {
        SetCamera(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::CameraData*>(String_To_UUID(data["targetCam"])));
        return true;
    }
}
