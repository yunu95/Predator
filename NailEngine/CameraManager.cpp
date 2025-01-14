#include "CameraManager.h"

#include "NailCamera.h"

#include "NailEngine.h"
#include "RenderTargetGroup.h"

LazyObjects<CameraManager> CameraManager::Instance;

void CameraManager::RegisterCamera(yunuGI::ICamera* adapter, std::shared_ptr<NailCamera> camera)
{
    if (cameraMap.empty())
    {
        this->mainCamera = camera;
    }

    this->cameraMap.insert({ adapter, camera });
}

void CameraManager::ChangeMainCamera(yunuGI::ICamera* adapter)
{
    this->mainCamera = this->cameraMap[adapter];
}

std::map<yunuGI::ICamera*, std::shared_ptr<NailCamera>>& CameraManager::GetCamearaList()
{
    return cameraMap;
}

void CameraManager::SetClearColor(yunuGI::Color color)
{
    auto& rtGroup =  NailEngine::Instance.Get().GetRenderTargetGroup();
    auto& rt = rtGroup[static_cast<int>(RENDER_TARGET_TYPE::FINAL)]->GetRT(0);
    rt.clearColor[0] = color.r;
    rt.clearColor[1] = color.g;
    rt.clearColor[2] = color.b;
    rt.clearColor[3] = color.a;
}

void CameraManager::SetWorldTM(yunuGI::ICamera* adapter, DirectX::SimpleMath::Matrix wtm)
{
    this->cameraMap[adapter]->SetWorldTM(wtm);
}

void CameraManager::SetVerticalFOV(yunuGI::ICamera* adapter, float fov)
{
    //mainCamera->SetVerticalFOV(fov);
    this->cameraMap[adapter]->SetVerticalFOV(fov);
}

float CameraManager::GetVerticalFOV(yunuGI::ICamera* adapter)
{
    return this->cameraMap[adapter]->GetVerticalFOV();
}

void CameraManager::SetNear(yunuGI::ICamera* adapter, float cameraNear)
{
    //mainCamera->SetNear(cameraNear);
    this->cameraMap[adapter]->SetNear(cameraNear);
}

float CameraManager::GetNear(yunuGI::ICamera* adapter)
{
    return this->cameraMap[adapter]->GetNear();
}

void CameraManager::SetFar(yunuGI::ICamera* adapter, float cameraFar)
{
    //mainCamera->SetFar(cameraFar);
    this->cameraMap[adapter]->SetFar(cameraFar);
}

float CameraManager::GetFar(yunuGI::ICamera* adapter)
{
    return this->cameraMap[adapter]->GetFar();
}

void CameraManager::SetResolution(yunuGI::ICamera* adapter, float width, float height)
{
    //mainCamera->SetResolution(width, height);
    this->cameraMap[adapter]->SetResolution(width, height);
}

void CameraManager::GetResolution(yunuGI::ICamera* adapter, float* width, float* height)
{
    this->cameraMap[adapter]->GetResolution(width, height);
}
DirectX::SimpleMath::Vector2 CameraManager::GetScreenPos(yunuGI::ICamera* adapter, const DirectX::SimpleMath::Vector3& worldPos)
{
    return this->cameraMap[adapter]->GetScreenPos(worldPos);
}

DirectX::SimpleMath::Matrix CameraManager::GetPTM90ByResolution(float width, float height, float farPlane, float nearPlane)
{
    return DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI / 2.f, width / height, nearPlane, farPlane);
}
