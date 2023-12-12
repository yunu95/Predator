#include "NailCamera.h"

LazyObjects<NailCamera> NailCamera::Instance;

void NailCamera::SetWorldTM(const DirectX::SimpleMath::Matrix wtm)
{
	this->wtm = wtm;
	this->vtm = this->wtm.Invert();
	this->ptm = DirectX::XMMatrixPerspectiveFovLH(this->fov, this->width / this->height, this->cameraNear, this->cameraFar);
	this->vtmOrtho = DirectX::XMMatrixOrthographicLH(this->width * 1.f, this->height * 1.f, this->cameraNear, this->cameraFar);
}

void NailCamera::SetFOV(float fov)
{
	this->fov = fov;
}

void NailCamera::SetNear(float cameraNear)
{
	this->cameraNear = cameraNear;
}

void NailCamera::SetFar(float cameraFar)
{
	this->cameraFar = cameraFar;
}

void NailCamera::SetResolution(float width, float height)
{
	this->width = width;
	this->height = height;
}

void NailCamera::SetAsMain()
{

}
