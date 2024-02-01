#pragma once
#include "YunutyEngine.h"
using namespace yunutyEngine;

class ShakyCam : public yunutyEngine::graphics::Camera
{
private:
    Quaternion rot;

protected:
    void Update()
    {
        float cameraSpeed = 10.0f;
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
        static auto lastMousePos = yunutyEngine::Input::getMouseScreenPosition();

        auto dMousePos = yunutyEngine::Input::getMouseScreenPosition() - lastMousePos;
        lastMousePos = yunutyEngine::Input::getMouseScreenPosition();

        if (yunutyEngine::Input::isKeyDown(KeyCode::MouseRightClick))
        {
            constexpr float rotationSpeedFactor = 0.003f;
            //auto euler = GetTransform()->rotation.Euler();
            //euler.y += dMousePos.x * rotationSpeedFactor;
            //euler.x += dMousePos.y * rotationSpeedFactor;
            Quaternion rot = GetTransform()->GetLocalRotation();
            Vector3d newForward = 
                rot.Forward() +
                dMousePos.x * rotationSpeedFactor * rot.Right() +
                -dMousePos.y * rotationSpeedFactor * rot.Up();
            Vector3d newRight =
                Vector3d::Cross(Vector3d::up, newForward);
            Vector3d newUp = 
                rot.Up() +
                -dMousePos.y * rotationSpeedFactor * -rot.Forward();
            //GetTransform()->rotation = Quaternion::MakeWithAxes(euler);
            GetTransform()->SetLocalRotation( Quaternion::MakeWithAxes(newRight, newUp, newForward));
        }

        GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition()+ deltaPosition.Normalized() * Time::GetDeltaTime() * cameraSpeed);
    }
};
