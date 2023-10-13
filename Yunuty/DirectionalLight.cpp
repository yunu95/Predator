#include "YunutyEngine.h"
#include "DirectionalLight.h"
#include "_YunuGIObjects.h"


yunutyEngine::graphics::DirectionalLight::DirectionalLight()
	:Renderable<yunuGI::IDirectionalLight>(_YunuGIObjects::SingleInstance().factory->CreateDirectionalLight({}))
{

}
