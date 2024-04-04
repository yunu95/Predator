#include "YunutyEngine.h"
#include "ParticleRenderer.h"
#include "_YunuGIObjects.h"
using namespace yunutyEngine::graphics;

yunutyEngine::graphics::ParticleRenderer::ParticleRenderer() :
	Renderable<yunuGI::IParticleRenderer>(_YunuGIObjects::SingleInstance().factory->CreateParticleRenderer({}))
{
}
