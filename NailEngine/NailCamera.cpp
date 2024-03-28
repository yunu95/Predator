#include "NailCamera.h"

void NailCamera::SetWorldTM(const DirectX::SimpleMath::Matrix wtm)
{
	this->cameraFar = 100.f;
	this->wtm = wtm;
	this->vtm = this->wtm.Invert();
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
	CreateFrustum();
}

void NailCamera::SetNear(float cameraNear)
{
	if (cameraNear <= 0.f)
	{
		return;
	}

	this->cameraNear = cameraNear;
	CreateFrustum();
}

void NailCamera::SetFar(float cameraFar)
{
	if (cameraFar <= 0.f || cameraFar <= this->cameraNear)
	{
		return;
	}

	this->cameraFar = cameraFar;
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

void NailCamera::CreateFrustum()
{
	this->frustum.CreateFromMatrix(this->frustum, ptm);
	this->frustum.Transform(this->frustum, wtm);
}
