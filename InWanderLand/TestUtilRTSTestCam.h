#pragma once
#include "YunutyEngine.h"
#include "DebugBeacon.h"

using namespace yunutyEngine;
namespace tests
{
    class RTSTestCam :public yunutyEngine::graphics::Camera
    {
    public:
        function<void(Vector3d)> groundLeftClickCallback{ [](Vector3d) {} };
        function<void(Vector3d)> groundLeftClickReleaseCallback{ [](Vector3d) {} };
        function<void(Vector3d)> groundRightClickCallback{ [](Vector3d) {} };
        function<void(Vector3d)> groundRightClickReleaseCallback{ [](Vector3d) {} };
        function<void(Vector3d)> groundHoveringClickCallback{ [](Vector3d) {} };
        function<void()> buttonCallbackDelete{ []() {} };
        function<void()> buttonCallback_X{ []() {} };
        function<void()> buttonCallbackSpace{ []() {} };
        void Update()
        {
            float cameraSpeed = 50.f;
            Camera::Update();
            Vector3d deltaDirection = Vector3d::zero;

            if (yunutyEngine::Input::isKeyDown(KeyCode::LShift))
                cameraSpeed = 20.0f;

            auto lastMousePos = yunutyEngine::Input::getMouseScreenPositionNormalized();
            if (yunutyEngine::Input::isKeyDown(KeyCode::A)/* || lastMousePos.x <= 0*/)
                deltaDirection -= Vector3d::right;
            if (yunutyEngine::Input::isKeyDown(KeyCode::D)/* || lastMousePos.x >= 1*/)
                deltaDirection += Vector3d::right;
            if (yunutyEngine::Input::isKeyDown(KeyCode::W)/* || lastMousePos.y <= 0*/)
                deltaDirection += Vector3d::forward;
            if (yunutyEngine::Input::isKeyDown(KeyCode::S)/* || lastMousePos.y >= 1*/)
                deltaDirection -= Vector3d::forward;

            GetTransform()->position += deltaDirection.Normalized() * Time::GetDeltaTime() * cameraSpeed;
            Quaternion quat = Quaternion(Vector3d(60, 0, 0));
            GetTransform()->rotation = quat;

            Vector3d front = GetTransform()->GetWorldRotation().Forward();
            auto distToXZPlane = abs(GetTransform()->GetWorldPosition().y);
            auto centeredPosition = Input::getMouseScreenPositionNormalized();
            centeredPosition.x -= 0.5;
            centeredPosition.y -= 0.5;
            centeredPosition.y *= -1;
            Vector3d projectedPoint = GetProjectedPoint(centeredPosition, distToXZPlane, Vector3d(0, 1, 0));

            if (Input::isKeyPushed(KeyCode::MouseLeftClick) || Input::isKeyPushed(KeyCode::MouseRightClick))
                DebugBeacon::PlaceBeacon(projectedPoint, Input::isKeyPushed(KeyCode::MouseLeftClick) ?
                    yunuGI::Color::red() : yunuGI::Color::blue(), { 0.2,0.2,0.2 });
            if (Input::isKeyPushed(KeyCode::MouseLeftClick))
            {
                if (groundLeftClickCallback) groundLeftClickCallback(projectedPoint);
            }
            else if (Input::isKeyLifted(KeyCode::MouseLeftClick))
            {
                if (groundLeftClickReleaseCallback) groundLeftClickReleaseCallback(projectedPoint);
            }
            if (Input::isKeyPushed(KeyCode::MouseRightClick))
            {
                if (groundRightClickCallback) groundRightClickCallback(projectedPoint);
            }
            else if (Input::isKeyLifted(KeyCode::MouseRightClick))
            {
                if (groundRightClickReleaseCallback) groundRightClickReleaseCallback(projectedPoint);
            }

            groundHoveringClickCallback(projectedPoint);
            if (yunutyEngine::Input::isKeyPushed(KeyCode::X))
                buttonCallback_X();
            if (yunutyEngine::Input::isKeyPushed(KeyCode::Space))
                buttonCallbackSpace();
            if (yunutyEngine::Input::isKeyPushed(KeyCode::Delete))
                buttonCallbackDelete();
        }
        float expectedPlaneDistance() { return abs(GetTransform()->GetWorldPosition().y); };
    private:
    };
}
