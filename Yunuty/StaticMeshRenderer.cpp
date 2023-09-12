#include "YunutyEngine.h"
#include "StaticMeshRenderer.h"
#include "_YunuGIObjects.h"
using namespace yunutyEngine::graphics;

yunutyEngine::graphics::StaticMeshRenderer::StaticMeshRenderer() :
    Renderable<yunuGI::IStaticMeshRenderer>(_YunuGIObjects::SingleInstance().factory->CreateStaticMesh({}))
{
}
