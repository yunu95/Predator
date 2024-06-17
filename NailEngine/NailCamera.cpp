#include "NailCamera.h"
#include "FrustumCullingManager.h"

#include <algorithm>

void NailCamera::GetCameraAreaXZ(DirectX::SimpleMath::Vector2& minPoint, DirectX::SimpleMath::Vector2& maxPoint)
{
    // XZ 평면에 대하여 카메라가 지금 비추고 있는 영역의 minPoint와 maxPoint를 반환해줌.
    DirectX::SimpleMath::Matrix matInv = ptm.Invert() * vtm.Invert();

    std::vector<DirectX::SimpleMath::Vector3> worldPos =
    {
        DirectX::XMVector3TransformCoord(DirectX::SimpleMath::Vector3(-1.f,1.f,0.f), matInv),
        DirectX::XMVector3TransformCoord(DirectX::SimpleMath::Vector3(1.f,1.f,0.f), matInv),
        DirectX::XMVector3TransformCoord(DirectX::SimpleMath::Vector3(1.f,-1.f,0.f), matInv),
        DirectX::XMVector3TransformCoord(DirectX::SimpleMath::Vector3(-1.f,-1.f,0.f), matInv),

        DirectX::XMVector3TransformCoord(DirectX::SimpleMath::Vector3(-1.f,1.f,1.f), matInv),
        DirectX::XMVector3TransformCoord(DirectX::SimpleMath::Vector3(1.f,1.f,1.f), matInv),
        DirectX::XMVector3TransformCoord(DirectX::SimpleMath::Vector3(1.f,-1.f,1.f), matInv),
        DirectX::XMVector3TransformCoord(DirectX::SimpleMath::Vector3(-1.f,-1.f,1.f), matInv),
    };

    minPoint.x = worldPos[0].x;
    minPoint.y = worldPos[0].z;

    maxPoint.x = worldPos[0].x;
    maxPoint.y = worldPos[0].z;

    for (auto& each : worldPos)
    {
        minPoint.x = std::min(minPoint.x, each.x);
        minPoint.y = std::min(minPoint.y, each.z);

        maxPoint.x = std::max(maxPoint.x, each.x);
        maxPoint.y = std::max(maxPoint.y, each.z);
    }
}

void NailCamera::SetWorldTM(const DirectX::SimpleMath::Matrix wtm)
{
    this->wtm = wtm;
    this->vtm = this->wtm.Invert();

#ifdef EDITOR

#else
	this->cameraNear = 10.f;
	this->cameraFar = 80.f;
#endif

    this->ptm = DirectX::XMMatrixPerspectiveFovLH(this->fov, this->width / this->height, this->cameraNear, this->cameraFar);
    this->vtmOrtho = DirectX::XMMatrixOrthographicLH(this->width * 1.f, this->height * 1.f, this->cameraNear, this->cameraFar);

    CreateFrustum();
}

void NailCamera::SetVerticalFOV(float fov)
{
    if (fov <= 0.f)
    {
        return;
    }

    this->fov = fov;
    this->ptm = DirectX::XMMatrixPerspectiveFovLH(this->fov, this->width / this->height, this->cameraNear, this->cameraFar);
    this->vtmOrtho = DirectX::XMMatrixOrthographicLH(this->width * 1.f, this->height * 1.f, this->cameraNear, this->cameraFar);

    CreateFrustum();
}

void NailCamera::SetNear(float cameraNear)
{
    if (cameraNear <= 0.f)
    {
        return;
    }

    this->cameraNear = cameraNear;
    this->ptm = DirectX::XMMatrixPerspectiveFovLH(this->fov, this->width / this->height, this->cameraNear, this->cameraFar);
    this->vtmOrtho = DirectX::XMMatrixOrthographicLH(this->width * 1.f, this->height * 1.f, this->cameraNear, this->cameraFar);

    CreateFrustum();
}

void NailCamera::SetFar(float cameraFar)
{
    if (cameraFar <= 0.f || cameraFar <= this->cameraNear)
    {
        return;
    }

    this->cameraFar = cameraFar;
    this->ptm = DirectX::XMMatrixPerspectiveFovLH(this->fov, this->width / this->height, this->cameraNear, this->cameraFar);
    this->vtmOrtho = DirectX::XMMatrixOrthographicLH(this->width * 1.f, this->height * 1.f, this->cameraNear, this->cameraFar);

    CreateFrustum();
}

void NailCamera::SetResolution(float width, float height)
{
    if (width <= 0.f || height <= 0.f)
    {
        return;
    }

    this->width = width;
    this->height = height;
    this->ptm = DirectX::XMMatrixPerspectiveFovLH(this->fov, this->width / this->height, this->cameraNear, this->cameraFar);
    this->vtmOrtho = DirectX::XMMatrixOrthographicLH(this->width * 1.f, this->height * 1.f, this->cameraNear, this->cameraFar);

    CreateFrustum();
}
float NailCamera::GetVerticalFOV()
{
    return this->fov;
}
float NailCamera::GetNear()
{
    return this->cameraNear;
}
float NailCamera::GetFar()
{
    return this->cameraFar;
}
void NailCamera::GetResolution(float* width, float* height)
{
    *width = this->width;
    *height = this->height;
}

void NailCamera::SetAsMain()
{

}

DirectX::SimpleMath::Matrix NailCamera::GetPTM90()
{
    return DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI / 2.f, this->width / this->height, this->cameraNear, this->cameraFar);
}
DirectX::SimpleMath::Vector3 NailCamera::GetViewPos(const DirectX::SimpleMath::Vector3& worldPos)
{
    return DirectX::XMVector3TransformCoord(worldPos, this->vtm);
    //DirectX::SimpleMath::Vector3 screenPos = DirectX::XMVector3TransformCoord(worldPos, this->vtm);
    //return screenPos;
}
DirectX::SimpleMath::Vector2 NailCamera::GetScreenPos(const DirectX::SimpleMath::Vector3& worldPos)
{
    //DirectX::SimpleMath::Vector3 screenPos = DirectX::XMVector3TransformCoord(worldPos, this->ptm * this->vtm);
    DirectX::SimpleMath::Vector3 screenPos = DirectX::XMVector3TransformCoord(worldPos, this->vtm * this->ptm);
    //screenPos.x = (screenPos.x + 1.f) * 0.5f * this->width;
    //screenPos.y = (1.f - screenPos.y) * 0.5f * this->height;

    return DirectX::SimpleMath::Vector2(screenPos.x, screenPos.y);
}

void NailCamera::CreateFrustum()
{
    //	FrustumCullingManager::Instance.Get().SetCameraChange(true);

    this->frustum.CreateFromMatrix(this->frustum, ptm);
    this->frustum.Transform(this->frustum, wtm);

    //FrustumCullingManager::Instance.Get().Wait();
    //FrustumCullingManager::Instance.Get().SetCameraChange(false);
    //FrustumCullingManager::Instance.Get().Init();
}
