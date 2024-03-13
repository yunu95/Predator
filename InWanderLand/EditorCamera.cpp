#include "InWanderLand.h"
#include "EditorCamera.h"
#include "EditorMath.h"
#include "MouseEvents.h"

#include "YunutyEngine.h"
#include "DebugMeshes.h"

#include <DirectXMath.h>

// 게임 엔진에서 사용할 Component 용 EditorCamera
class EditorCameraComponent
    : public yunutyEngine::graphics::Camera
{
    friend class application::editor::EditorCamera;
private:
    Vector3d euler;
};

namespace application
{
    namespace editor
    {
        float EditorCamera::min_Speed = 0.002f;
        float EditorCamera::max_Speed = 2.0f;

        void EditorCamera::Initialize(yunutyEngine::graphics::Camera* gameCam)
        {
            // Game 카메라 세팅
            this->gameCam = gameCam;

            // Editor 카메라를 Scene에 생성하고 해당 카메라를 메인으로 변경
            auto scene = yunutyEngine::Scene::getCurrentScene();
            auto ecc = scene->AddGameObject()->AddComponent<EditorCameraComponent>();
            ecc->SetCameraMain();
            auto ts = ecc->GetTransform();
            if (gameCam)
            {
                auto gcts = gameCam->GetTransform();
                ts->SetWorldPosition(gcts->GetWorldPosition());
                ts->SetWorldRotation(gcts->GetWorldRotation());
                cameraPState = CameraPerspectiveState::Game;
            }
            else
            {
                ts->SetWorldPosition(Vector3d{ 0, 10, -5 });
                ts->SetWorldRotation(Vector3d{ 60, 0, 0 });
                cameraPState = CameraPerspectiveState::Free;
            }

            editorCam = ecc;
            cameraTState = CameraTypeState::Editor;

            auto ecPos = ts->GetWorldPosition();
            position.x = ecPos.x;
            position.y = ecPos.y;
            position.z = ecPos.z;

            auto rot = ts->GetWorldRotation();
            glm::quat glmRot = glm::quat(rot.w, -rot.x, -rot.y, rot.z);

            pitch = -glm::pitch(glmRot);
            yaw = -glm::yaw(glmRot);

            forwardDirection = GetForwardDirection();
            rightDirection = GetRightDirection();

            /// Gizmo
            //axisGizmo = scene->AddGameObject();
            //auto xAxis = scene->AddGameObject(axisGizmo);
            //auto xTF = xAxis->GetTransform();
            //xTF->scale.x = 10;
            //xTF->position.x = 5;
            //AttachDebugMesh(xAxis, DebugMeshType::Cube, yunuGI::Color::red(), false);
            //auto yAxis = scene->AddGameObject(axisGizmo);
            //auto yTF = yAxis->GetTransform();
            //yTF->scale.y = 10;
            //yTF->position.y = 5;
            //AttachDebugMesh(yAxis, DebugMeshType::Cube, yunuGI::Color::green(), false);
            //auto zAxis = scene->AddGameObject(axisGizmo);
            //auto zTF = zAxis->GetTransform();
            //zTF->scale.z = 10;
            //zTF->position.z = 5;
            //AttachDebugMesh(zAxis, DebugMeshType::Cube, yunuGI::Color::blue(), false);

            //UpdateGizmo();
        }

        void EditorCamera::OnEvent(EditorEvents& event)
        {
            if (event.GetEventType() == EventType::MouseMove)
            {
                mousePos.x = static_cast<MouseMoveEvent*>(&event)->GetPosX();
                mousePos.y = static_cast<MouseMoveEvent*>(&event)->GetPosY();
            }
        }

        void EditorCamera::SwitchCam()
        {
            switch (cameraTState)
            {
            case application::editor::CameraTypeState::Editor:
            {
                if (gameCam)
                {
                    cameraTState = CameraTypeState::Game;
                    gameCam->SetCameraMain();
                }
                break;
            }
            case application::editor::CameraTypeState::Game:
            {
                cameraTState = CameraTypeState::Editor;
                editorCam->SetCameraMain();
                break;
            }
            default:
                break;
            }
        }

        void EditorCamera::Update(float ts)
        {
            yunuGI::Vector2 deltaPos;
            deltaPos.x = (mousePos.x - beforeMousePos.x) * 0.002f;
            deltaPos.y = (mousePos.y - beforeMousePos.y) * 0.002f;


            if (inputUpdate)
            {
                if (eim.IsKeyboardUp(KeyCode::C))
                {
                    SwitchCam();
                }

                if (eim.IsMouseButtonDown(MouseCode::Right))
                {
                    switch (cameraTState)
                    {
                    case application::editor::CameraTypeState::Editor:
                    {
                        const float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
                        const float speed = GetCameraSpeed();

                        switch (cameraPState)
                        {
                        case application::editor::CameraPerspectiveState::Free:
                        {
                            if (eim.IsKeyboardDown(KeyCode::Q))
                            {
                                positionDelta.y -= speed * yunuGI::Vector3(0.f, yawSign, 0.f).y;
                            }
                            if (eim.IsKeyboardDown(KeyCode::E))
                            {
                                positionDelta.y += speed * yunuGI::Vector3(0.f, yawSign, 0.f).y;
                            }
                            if (eim.IsKeyboardDown(KeyCode::S))
                            {
                                positionDelta.x -= speed * forwardDirection.x;
                                positionDelta.y -= speed * forwardDirection.y;
                                positionDelta.z -= speed * forwardDirection.z;
                            }
                            if (eim.IsKeyboardDown(KeyCode::W))
                            {
                                positionDelta.x += speed * forwardDirection.x;
                                positionDelta.y += speed * forwardDirection.y;
                                positionDelta.z += speed * forwardDirection.z;
                            }
                            if (eim.IsKeyboardDown(KeyCode::A))
                            {
                                positionDelta.x -= speed * rightDirection.x;
                                positionDelta.y -= speed * rightDirection.y;
                                positionDelta.z -= speed * rightDirection.z;
                            }
                            if (eim.IsKeyboardDown(KeyCode::D))
                            {
                                positionDelta.x += speed * rightDirection.x;
                                positionDelta.y += speed * rightDirection.y;
                                positionDelta.z += speed * rightDirection.z;
                            }

                            float maxRate = 0.12f;
                            yawDelta += glm::clamp(yawSign * deltaPos.x * rotationSpeed, -maxRate, maxRate);
                            pitchDelta += glm::clamp(deltaPos.y * rotationSpeed, -maxRate, maxRate);
                            break;
                        }
                        case application::editor::CameraPerspectiveState::Game:
                        {
                            if (eim.IsKeyboardDown(KeyCode::Q))
                            {
                                positionDelta.y -= speed * yunuGI::Vector3(0.f, yawSign, 0.f).y;
                            }
                            if (eim.IsKeyboardDown(KeyCode::E))
                            {
                                positionDelta.y += speed * yunuGI::Vector3(0.f, yawSign, 0.f).y;
                            }
                            if (eim.IsKeyboardDown(KeyCode::S))
                            {
                                positionDelta.x -= speed * forwardDirection.x;
                                positionDelta.z -= speed * forwardDirection.z;
                            }
                            if (eim.IsKeyboardDown(KeyCode::W))
                            {
                                positionDelta.x += speed * forwardDirection.x;
                                positionDelta.z += speed * forwardDirection.z;
                            }
                            if (eim.IsKeyboardDown(KeyCode::A))
                            {
                                positionDelta.x -= speed * rightDirection.x;
                                positionDelta.z -= speed * rightDirection.z;
                            }
                            if (eim.IsKeyboardDown(KeyCode::D))
                            {
                                positionDelta.x += speed * rightDirection.x;
                                positionDelta.z += speed * rightDirection.z;
                            }
                            break;
                        }
                        default:
                            break;
                        }

                        const float distance = glm::distance(glm::vec3{ focalPos.x, focalPos.y, -focalPos.z }, glm::vec3{ position.x, position.y, -position.z });
                        focalPos.x = position.x + GetForwardDirection().x * distance;
                        focalPos.y = position.y + GetForwardDirection().y * distance;
                        focalPos.z = position.z + GetForwardDirection().z * distance;
                        focalDistance = distance;
                        break;
                    }
                    case application::editor::CameraTypeState::Game:
                    {
                        // 동작하지 않음
                        break;
                    }
                    default:
                        break;
                    }
                }
            }

            beforeMousePos = mousePos;

            UpdateCameraView();
        }

        void EditorCamera::SetGamePerspective()
        {
            if (cameraPState == CameraPerspectiveState::Game)
            {
                return;
            }
            else
            {
                if (gameCam)
                {
                    auto ts = editorCam->GetTransform();
                    auto gcts = gameCam->GetTransform();
                    auto pos = ts->GetWorldPosition();
                    pos.y = gcts->GetWorldPosition().y;
                    ts->SetWorldPosition(pos);
                    position.y = pos.y;
                    positionDelta = yunuGI::Vector3();

                    auto rot = gcts->GetWorldRotation();
                    ts->SetWorldRotation(rot);
                    glm::quat glmRot = glm::quat(rot.w, -rot.x, -rot.y, rot.z);

                    pitch = -glm::pitch(glmRot);
                    yaw = -glm::yaw(glmRot);

                    pitchDelta = 0.0f;
                    yawDelta = 0.0f;

                    forwardDirection = GetForwardDirection();
                    rightDirection = GetRightDirection();

                    cameraPState = CameraPerspectiveState::Game;
                }
                return;
            }
        }

        void EditorCamera::SetFreePerspective()
        {
            if (cameraPState == CameraPerspectiveState::Free)
            {
                return;
            }
            else
            {
                cameraPState = CameraPerspectiveState::Free;
                return;
            }
        }

        float EditorCamera::GetCameraSpeed() const
        {
            float speed = defaultMoveSpeed;
            if (eim.IsKeyboardDown(KeyCode::Control))
            {
                speed /= 2 - glm::log(defaultMoveSpeed);
            }

            if (eim.IsKeyboardDown(KeyCode::Shift))
            {
                speed *= 2 - glm::log(defaultMoveSpeed);
            }

            return glm::clamp(speed, min_Speed, max_Speed);
        }

        yunuGI::Vector3 EditorCamera::GetUpDirection() const
        {
            auto up = editorCam->GetTransform()->GetWorldRotation().Up();
            return yunuGI::Vector3(up.x, up.y, up.z);
        }

        yunuGI::Vector3 EditorCamera::GetRightDirection() const
        {
            auto right = editorCam->GetTransform()->GetWorldRotation().Right();
            return yunuGI::Vector3(right.x, right.y, right.z);
        }

        yunuGI::Vector3 EditorCamera::GetForwardDirection() const
        {
            auto forward = editorCam->GetTransform()->GetWorldRotation().Forward();
            return yunuGI::Vector3(forward.x, forward.y, forward.z);
        }

        yunuGI::Quaternion EditorCamera::GetOrientation() const
        {
            auto rot = editorCam->GetTransform()->GetWorldRotation();
            return yunuGI::Quaternion(rot.x, rot.y, rot.z, rot.w);
        }

        yunuGI::ICamera& EditorCamera::GetGI()
        {
            switch (cameraTState)
            {
            case application::editor::CameraTypeState::Editor:
                return editorCam->GetGI();
            case application::editor::CameraTypeState::Game:
                if (gameCam)
                {
                    return gameCam->GetGI();
                }
                break;
            default:
                break;
            }
        }

        void EditorCamera::ChangeVTM(const yunuGI::Matrix4x4& vtm)
        {
            switch (cameraTState)
            {
            case application::editor::CameraTypeState::Editor:
            {
                auto gvtm = math::ConvertVTM(vtm);
                auto gwtm = glm::inverse(gvtm);

                glm::vec3 scale;
                glm::quat rotation;
                glm::vec3 position;
                glm::vec3 skew;
                glm::vec4 perspective;

                glm::decompose(gwtm, scale, rotation, position, skew, perspective);

                pitch = -glm::pitch(rotation);
                yaw = -glm::yaw(rotation);

                positionDelta = yunuGI::Vector3();

                pitchDelta = 0.0f;
                yawDelta = 0.0f;

                UpdateCameraView();
                break;
            }
            case application::editor::CameraTypeState::Game:
                break;
            default:
                break;
            }
        }

        void EditorCamera::UpdateCameraView()
        {
            glm::vec3 glmPos = glm::vec3(position.x, position.y, -position.z);

            focalDistance = glm::distance(glmPos, glm::vec3{ focalPos.x, focalPos.y, -focalPos.z });

            //damping for smooth camera
            yawDelta *= 0.6f;
            pitchDelta *= 0.6f;
            positionDelta.x *= 0.8f;
            positionDelta.y *= 0.8f;
            positionDelta.z *= 0.8f;

            position.x += positionDelta.x;
            position.y += positionDelta.y;
            position.z += positionDelta.z;
            yaw += yawDelta;
            pitch += pitchDelta;

            if (editorCam)
            {
                editorCam->GetTransform()->SetWorldPosition(Vector3d(position.x, position.y, position.z));
                editorCam->GetTransform()->SetWorldRotation(Quaternion(Vector3d(pitch * yunutyEngine::math::Rad2Deg, yaw * yunutyEngine::math::Rad2Deg, 0.0)));

                forwardDirection = GetForwardDirection();
                rightDirection = GetRightDirection();
            }

            //UpdateGizmo();
        }

        void EditorCamera::UpdateGizmo()
        {
            auto ts = yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform();
            axisGizmo->GetTransform()->SetWorldPosition(ts->GetWorldPosition() + ts->GetWorldRotation().Forward() * 200 + ts->GetWorldRotation().Up() * 65 + ts->GetWorldRotation().Right() * 130);
        }

        yunuGI::Matrix4x4 EditorCamera::GetWTM() const
        {
            switch (cameraTState)
            {
            case application::editor::CameraTypeState::Editor:
            {
                auto wm = editorCam->GetTransform()->GetWorldTM();
                return *reinterpret_cast<yunuGI::Matrix4x4*>(&wm);
            }
            case application::editor::CameraTypeState::Game:
            {
                if (gameCam)
                {
                    auto wm = gameCam->GetTransform()->GetWorldTM();
                    return *reinterpret_cast<yunuGI::Matrix4x4*>(&wm);
                }
            }
            default:
                return yunuGI::Matrix4x4();
            }
        }

        yunuGI::Matrix4x4 EditorCamera::GetVTM() const
        {
            return math::GetInverseMatrix(GetWTM());
        }

        yunuGI::Matrix4x4 EditorCamera::GetPTM() const
        {
            switch (cameraTState)
            {
            case application::editor::CameraTypeState::Editor:
            {
                float width;
                float height;
                editorCam->GetGI().GetResolution(&width, &height);
                DirectX::XMMATRIX pm = DirectX::XMMatrixPerspectiveFovLH(editorCam->GetGI().GetVerticalFOV(), width / height, editorCam->GetGI().GetNear(), editorCam->GetGI().GetFar());
                return *reinterpret_cast<yunuGI::Matrix4x4*>(&pm);
            }
            case application::editor::CameraTypeState::Game:
            {
                if (gameCam)
                {
                    float width;
                    float height;
                    gameCam->GetGI().GetResolution(&width, &height);
                    DirectX::XMMATRIX pm = DirectX::XMMatrixPerspectiveFovLH(gameCam->GetGI().GetVerticalFOV(), width / height, gameCam->GetGI().GetNear(), gameCam->GetGI().GetFar());
                    return *reinterpret_cast<yunuGI::Matrix4x4*>(&pm);
                }
            }
            default:
                return yunuGI::Matrix4x4();
            }
        }
    }
}
