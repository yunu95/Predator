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
Vector3d yunutyEngine::graphics::Camera::GetProjectedPoint(const Vector2d& normalizedScreenPosition, double distance, const Vector3d& norm)
{
    // 카메라의 Forward 벡터와 평면의 normal 벡터의 내적이 음수일 경우에만 평면에 사영이 가능함
    // 사영이 불가능한 경우, 카메라의 위치 값으로 데이터를 반환하도록 함
    auto forward = GetTransform()->GetWorldRotation().Forward();
    auto right = GetTransform()->GetWorldRotation().Right();
    auto up = GetTransform()->GetWorldRotation().Up();
    auto resolution = graphics::Renderer::SingleInstance().GetResolution();
    double tanFOV = tan(yunuGICamera.Get()->GetVerticalFOV() * 0.5);

    if (Vector3d::Dot(forward, norm) >= 0)
    {
        return GetTransform()->GetWorldPosition() - forward * 10;
    }

    // 스크린 좌표계 위치의 방향 벡터
    auto lineDirection = forward + 
        right * 2 * normalizedScreenPosition.x * tanFOV * resolution.x / resolution.y +
        up * 2 * normalizedScreenPosition.y * tanFOV;
    lineDirection = lineDirection.Normalized();

    // 카메라 위치의 평면 normal 방향 사영된 점
    auto planePoint = GetTransform()->GetWorldPosition() - norm.Normalized() * distance;

    // 카메라 위치에서 lineDirection 방향의 직선과 평면의 교점 방정식을 통한
    // World 상의 좌표 계산
    float scalarT = -distance / Vector3d::Dot(lineDirection, norm);

    return GetTransform()->GetWorldPosition() + scalarT * lineDirection;
}

