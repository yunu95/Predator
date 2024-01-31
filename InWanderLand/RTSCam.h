#pragma once
#include "YunutyEngine.h"
#include "DebugBeacon.h"
using namespace yunutyEngine;

class RTSCam :public yunutyEngine::graphics::Camera
{
public:
    bool roamingMode = false;
    function<void(Vector3d)> groundLeftClickCallback{ [](Vector3d) {} };
    function<void(Vector3d)> groundLeftClickReleaseCallback{ [](Vector3d) {} };
    function<void(Vector3d)> groundRightClickCallback{ [](Vector3d) {} };
    function<void(Vector3d)> groundRightClickReleaseCallback{ [](Vector3d) {} };
    function<void(Vector3d)> groundHoveringClickCallback{ [](Vector3d) {} };
    function<void()> deleteButtonCallback{ []() {} };
    function<void()> tabButtonCallback{ []() {} };
    function<void()> xButtonCallback{ []() {} };
    void Update()
    {
        if (Input::isKeyPushed(KeyCode::Tab))
            roamingMode = !roamingMode;

        if (yunutyEngine::Input::isKeyPushed(KeyCode::Delete))
            deleteButtonCallback();

        if (yunutyEngine::Input::isKeyPushed(KeyCode::Tab))
            tabButtonCallback();

        if (yunutyEngine::Input::isKeyPushed(KeyCode::X))
            xButtonCallback();

        if (!roamingMode)
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

            GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition()+ deltaDirection.Normalized() * Time::GetDeltaTime() * cameraSpeed);
            Quaternion quat = Quaternion(Vector3d(60, 0, 0));
            GetTransform()->SetLocalRotation( quat);
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

            if (yunutyEngine::Input::isKeyDown(KeyCode::Alphabet_E))
                deltaPosition += Vector3d::up;


            auto lastMousePos = Vector2d::zero;
            auto dMousePos = Vector3d::zero;

            //if (yunutyEngine::Input::isKeyPushed(KeyCode::MouseRightClick))
            //{
            //    lastMousePos = yunutyEngine::Input::getMouseScreenPosition();
            //}

            //if (yunutyEngine::Input::isKeyDown(KeyCode::MouseRightClick))
            //{
            //    dMousePos = yunutyEngine::Input::getMouseScreenPosition();
            //}

            if (yunutyEngine::Input::isKeyDown(KeyCode::LeftArrow))
            {
                //Quaternion& rot = GetTransform()->rotation;
                //auto euler = rot.Euler();
                //rot = Quaternion{ Vector3d{euler.x,euler.y -= (20.0f * Time::GetDeltaTime())   , euler.z} };
                euler.y -= (20.0f * Time::GetDeltaTime());
            }
            if (yunutyEngine::Input::isKeyDown(KeyCode::RightArrow))
            {
                //Quaternion& rot = GetTransform()->rotation;
                //auto euler = rot.Euler();
                //rot = Quaternion{ Vector3d{euler.x,euler.y += (20.0f * Time::GetDeltaTime())   , euler.z} };
                euler.y += (20.0f * Time::GetDeltaTime());
            }
            if (yunutyEngine::Input::isKeyDown(KeyCode::UpArrow))
            {
                //Quaternion& rot = GetTransform()->rotation;
                //auto euler = rot.Euler();
                //rot = Quaternion{ Vector3d{euler.x -= (20.0f * Time::GetDeltaTime()) ,euler.y , euler.z   } };
                euler.x -= (20.0f * Time::GetDeltaTime());
            }
            if (yunutyEngine::Input::isKeyDown(KeyCode::DownArrow))
            {
                //Quaternion& rot = GetTransform()->rotation;
                //auto euler = rot.Euler();
                //rot = Quaternion{ Vector3d{euler.x += (20.0f * Time::GetDeltaTime()) ,euler.y , euler.z   } };
                euler.x += (20.0f * Time::GetDeltaTime());
            }
            GetTransform()->SetLocalRotation( Quaternion{ euler });

            //if (yunutyEngine::Input::is
            // 
            // 
            // 
            // Down(KeyCode::MouseRightClick))
            //{
            //    constexpr float rotationSpeedFactor = 0.003f;
            //    //auto euler = GetTransform()->rotation.Euler();
            //    //euler.y += dMousePos.x * rotationSpeedFactor;
            //    //euler.x += dMousePos.y * rotationSpeedFactor;
            //    Quaternion& rot = GetTransform()->rotation;
            //    Vector3d newForward =
            //        rot.Forward() +
            //        dMousePos.x * rotationSpeedFactor * rot.Right() +
            //        -dMousePos.y * rotationSpeedFactor * rot.Up();
            //    //Vector3d newRight =
            //    //    //Vector3d::right +
            //    //    rot.Right() +
            //    //    dMousePos.x * rotationSpeedFactor * -rot.Forward();
            //    Vector3d newUp = rot.Up() + -dMousePos.y * rotationSpeedFactor * -rot.Forward();
            //    Vector3d newRight = Vector3d::Cross(Vector3d::up, newForward);
            //    //GetTransform()->rotation = Quaternion::MakeWithAxes(euler);
            //    GetTransform()->rotation = Quaternion::MakeWithAxes(newRight, newUp, newForward);
            //}

            GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition()+ deltaPosition.Normalized() * Time::GetDeltaTime() * cameraSpeed);
        }

        if (!roamingMode)
        {
            auto front = yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldRotation().Forward();
            auto distToXZPlane = abs(yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition().y);
            auto centeredPosition = Input::getMouseScreenPositionNormalized();
            centeredPosition.x -= 0.5;
            centeredPosition.y -= 0.5;
            centeredPosition.y *= -1;
            auto projectedPoint = yunutyEngine::graphics::Camera::GetMainCamera()->GetProjectedPoint(centeredPosition, distToXZPlane, Vector3d(0, 1, 0));

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
    }
private:
    float expectedPlaneDistance() { return abs(GetTransform()->GetWorldPosition().y); };
    Vector3d euler;
};

