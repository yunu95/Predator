#include "NailCamera.h"

LazyObjects<NailCamera> NailCamera::Instance;

void NailCamera::SetWorldTM(const DirectX::SimpleMath::Matrix wtm)
{
	this->wtm = wtm;
	this->vtm = this->wtm.Invert();
	this->ptm = DirectX::XMMatrixPerspectiveFovLH(this->fov, this->width / this->height, this->cameraNear, this->cameraFar);
	//matOrthographicProjection = DirectX::XMMatrixOrthographicLH(m_width * m_scale, m_height * m_scale, m_near, m_far);
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
