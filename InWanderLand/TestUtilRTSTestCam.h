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
        void Update()
        {
            Camera::Update();
            GetTransform()->SetWorldRotation(Vector3d{90,0,0});

            float cameraSpeed = 1.0f;
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


            auto lastMousePos = Vector2d::zero;
            auto dMousePos = Vector3d::zero;

            GetTransform()->position += deltaPosition.Normalized() * Time::GetDeltaTime() * cameraSpeed;

            Vector3d projectedPoint;
            auto centeredPosition = Input::getMouseScreenPositionNormalized();
            centeredPosition.x -= 0.5;
            centeredPosition.y -= 0.5;
            centeredPosition.y *= -1;
            projectedPoint = GetProjectedPoint(centeredPosition, expectedPlaneDistance());

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
        }
        float expectedPlaneDistance() { return abs(GetTransform()->GetWorldPosition().y); };
    private:
    };
}
