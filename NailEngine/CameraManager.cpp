#include "CameraManager.h"

#include "NailCamera.h"

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

void CameraManager::SetWorldTM(yunuGI::ICamera* adapter, DirectX::SimpleMath::Matrix wtm)
{
	this->cameraMap[adapter]->SetWorldTM(wtm);
}

void CameraManager::SetVerticalFOV(yunuGI::ICamera* adapter, float fov)
{
	this->cameraMap[adapter]->SetVerticalFOV(fov);
}

float CameraManager::GetVerticalFOV(yunuGI::ICamera* adapter)
{
	return this->cameraMap[adapter]->GetVerticalFOV();
}

void CameraManager::SetNear(yunuGI::ICamera* adapter, float cameraNear)
{
	this->cameraMap[adapter]->SetNear(cameraNear);
}

float CameraManager::GetNear(yunuGI::ICamera* adapter)
{
	return this->cameraMap[adapter]->GetNear();
}

void CameraManager::SetFar(yunuGI::ICamera* adapter, float cameraFar)
{
	this->cameraMap[adapter]->SetFar(cameraFar);
}

float CameraManager::GetFar(yunuGI::ICamera* adapter)
{
	return this->cameraMap[adapter]->GetFar();
}

void CameraManager::SetResolution(yunuGI::ICamera* adapter, float width, float height)
{
	this->cameraMap[adapter]->SetResolution(width, height);
}

void CameraManager::GetResolution(yunuGI::ICamera* adapter, float* width, float* height)
{
	this->cameraMap[adapter]->GetResolution(width, height);
}

DirectX::SimpleMath::Matrix CameraManager::GetPTM90ByResolution(float width, float height)
{
	return DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI / 2.f, width / height, 0.001f, 10.f);
}
