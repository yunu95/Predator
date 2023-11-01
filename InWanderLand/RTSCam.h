#pragma once
#include "YunutyEngine.h"
#include "DebugBeacon.h"
using namespace yunutyEngine;

class RTSCam :public yunutyEngine::graphics::Camera
{
public:
    bool roamingMode = false;
    function<void(Vector3d)> groundLeftClickCallback{[](Vector3d) {}};
    function<void(Vector3d)> groundRightClickCallback{[](Vector3d) {}};
    function<void(Vector3d)> groundHoveringClickCallback{[](Vector3d) {}};
    void Update()
    {
        if (Input::isKeyPushed(KeyCode::Tab))
            roamingMode = !roamingMode;

        if (!roamingMode)
        {
            float cameraSpeed = 5.0f;
            Camera::Update();
            Vector3d deltaDirection = Vector3d::zero;

            if (yunutyEngine::Input::isKeyDown(KeyCode::LShift))
                cameraSpeed = 20.0f;

            auto lastMousePos = yunutyEngine::Input::getMouseScreenPositionNormalized();
            if (yunutyEngine::Input::isKeyDown(KeyCode::A) || lastMousePos.x < 0)
                deltaDirection -= Vector3d::right;
            if (yunutyEngine::Input::isKeyDown(KeyCode::D) || lastMousePos.x > 1)
                deltaDirection += Vector3d::right;
            if (yunutyEngine::Input::isKeyDown(KeyCode::W) || lastMousePos.y < 0)
                deltaDirection += Vector3d::forward;
            if (yunutyEngine::Input::isKeyDown(KeyCode::S) || lastMousePos.y > 1)
                deltaDirection -= Vector3d::forward;

            GetTransform()->position += deltaDirection.Normalized() * Time::GetDeltaTime() * cameraSpeed;
            Quaternion quat = Quaternion::MakeWithForwardUp(Vector3d::down,Vector3d::forward);
            GetTransform()->rotation = quat;
        }
        else
        {
            float cameraSpeed = 1.0f;
            Camera::Update();
            Vector3d deltaPosition = Vector3d::zero;

            if (yunutyEngine::Input::isKeyDown(KeyCode::LShift))
                cameraSpeed = 20.0f;

            if (yunutyEngine::Input::isKeyDown(KeyCode::A))
                deltaPosition -= GetTransform()->GetWorldRotation().Right();
            if (yunutyEngine::Input::isKeyDown(KeyCode::D))
                deltaPosition += GetTransform()->GetWorldRotation().Right();
            if (yunutyEngine::Input::isKeyDown(KeyCode::W))
                deltaPosition += GetTransform()->GetWorldRotation().Forward();
            if (yunutyEngine::Input::isKeyDown(KeyCode::S))
                deltaPosition -= GetTransform()->GetWorldRotation().Forward();
            if (yunutyEngine::Input::isKeyDown(KeyCode::Q))
                deltaPosition -= Vector3d::up;
            if (yunutyEngine::Input::isKeyDown(KeyCode::E))
                deltaPosition += Vector3d::up;
            static auto lastMousePos = yunutyEngine::Input::getMouseScreenPosition();

            auto dMousePos = yunutyEngine::Input::getMouseScreenPosition() - lastMousePos;
            lastMousePos = yunutyEngine::Input::getMouseScreenPosition();

            if (yunutyEngine::Input::isKeyDown(KeyCode::MouseRightClick))
            {
                constexpr float rotationSpeedFactor = 0.003f;
                //auto euler = GetTransform()->rotation.Euler();
                //euler.y += dMousePos.x * rotationSpeedFactor;
                //euler.x += dMousePos.y * rotationSpeedFactor;
                Quaternion& rot = GetTransform()->rotation;
                Vector3d newForward =
                    rot.Forward() +
                    dMousePos.x * rotationSpeedFactor * rot.Right() +
                    -dMousePos.y * rotationSpeedFactor * rot.Up();
                //Vector3d newRight =
                //    //Vector3d::right +
                //    rot.Right() +
                //    dMousePos.x * rotationSpeedFactor * -rot.Forward();
                Vector3d newUp = rot.Up() + -dMousePos.y * rotationSpeedFactor * -rot.Forward();
                Vector3d newRight = Vector3d::Cross(Vector3d::up, newForward);
                //GetTransform()->rotation = Quaternion::MakeWithAxes(euler);
                GetTransform()->rotation = Quaternion::MakeWithAxes(newRight, newUp, newForward);
            }

            GetTransform()->position += deltaPosition.Normalized() * Time::GetDeltaTime() * cameraSpeed;
        }

        if (!roamingMode)
        {
            Vector3d projectedPoint;
            auto resolution = graphics::Renderer::SingleInstance().GetResolution();
            auto centeredPosition = Input::getMouseScreenPositionNormalized();
            centeredPosition.x -= 0.5;
            centeredPosition.y -= 0.5;
            centeredPosition.y *= -1;
            // 카메라 해상도가 1280, 800일때 near plane의 가로가 1.28, 0.8임.
            auto forward = GetTransform()->GetWorldRotation().Forward();
            auto right = GetTransform()->GetWorldRotation().Right();
            auto up = GetTransform()->GetWorldRotation().Up();
            projectedPoint = GetTransform()->GetWorldPosition() +
                forward * expectedPlaneDistance() +
                right * centeredPosition.x * 0.001 * resolution.x * expectedPlaneDistance() +
                up * centeredPosition.y * 0.001 * resolution.y * expectedPlaneDistance();

            if (Input::isKeyPushed(KeyCode::MouseLeftClick) || Input::isKeyPushed(KeyCode::MouseRightClick))
                DebugBeacon::PlaceBeacon(projectedPoint, Input::isKeyPushed(KeyCode::MouseLeftClick) ?  yunuGI::Color::red() : yunuGI::Color::blue(), { 0.2,0.2,0.2 });
            if (Input::isKeyPushed(KeyCode::MouseLeftClick))
                groundLeftClickCallback(projectedPoint);
            else if (Input::isKeyPushed(KeyCode::MouseRightClick))
                groundRightClickCallback(projectedPoint);

            groundHoveringClickCallback(projectedPoint);
        }
    }
private:
    float expectedPlaneDistance() { return abs(GetTransform()->GetWorldPosition().y); };
};

