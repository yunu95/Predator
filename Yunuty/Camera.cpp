#include "YunutyEngine.h"
#include "Camera.h"
#include "_YunuGIObjects.h"
using namespace yunutyEngine::graphics;

Camera* yunutyEngine::graphics::Camera::mainCamera = nullptr;
yunutyEngine::graphics::Camera::Camera()
{
    if (mainCamera == nullptr)
        mainCamera = this;
    yunuGICamera = _YunuGIObjects::SingleInstance().factory->CreateCamera({});
}
void yunutyEngine::graphics::Camera::Update()
{
    GetTransform()->scale = Vector3d::one;
    yunuGICamera->SetWorldTM(GetTransform()->GetWorldTM());
}
void yunutyEngine::graphics::Camera::SetCameraMain()
{
    mainCamera = this;
    yunuGICamera->SetAsMain();
}
yunuGI::ICamera& yunutyEngine::graphics::Camera::GetGI()
{
    return *yunuGICamera.Get();
}
Camera* yunutyEngine::graphics::Camera::GetMainCamera()
{
    return mainCamera;
}
Vector3d yunutyEngine::graphics::Camera::GetProjectedPoint(const Vector2d& normalizedScreenPosition, double distance)
{
    // 카메라 해상도가 1280, 800일때 near plane의 가로가 1.28, 0.8임.
    auto forward = GetTransform()->GetWorldRotation().Forward();
    auto right = GetTransform()->GetWorldRotation().Right();
    auto up = GetTransform()->GetWorldRotation().Up();
    auto resolution = graphics::Renderer::SingleInstance().GetResolution();
    double tanFOV = tan(yunuGICamera.Get()->GetVerticalFOV() * 0.5);
    return GetTransform()->GetWorldPosition() +
        forward * distance +
        right * 2 * normalizedScreenPosition.x * tanFOV * distance * resolution.x / resolution.y +
        up * 2 * normalizedScreenPosition.y * tanFOV * distance;
}
