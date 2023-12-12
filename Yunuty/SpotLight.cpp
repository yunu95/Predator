#include "YunutyEngine.h"
#include "SpotLight.h"
#include "_YunuGIObjects.h"


yunutyEngine::graphics::SpotLight::SpotLight()
	:Renderable<yunuGI::ISpotLight>(_YunuGIObjects::SingleInstance().factory->CreateSpotLight({}))
{

}
