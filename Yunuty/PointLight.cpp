#include "YunutyEngine.h"
#include "PointLight.h"
#include "_YunuGIObjects.h"

yunutyEngine::graphics::PointLight::PointLight()
	:Renderable<yunuGI::IPointLight>(_YunuGIObjects::SingleInstance().factory->CreatePointLight({}))
{

}
