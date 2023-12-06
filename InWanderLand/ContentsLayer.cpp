#include "ContentsLayer.h"

#include "YunutyEngine.h"
#include "ShakyCam.h"
#include "RTSCam.h"   
#include "DebugTilePlane.h"
#include "DebugBeacon.h"
#include "DebugMeshes.h"

#include "TestColliderEnableDeath.h"
#include "TestNavAgentEnableDeath.h"

#include <d3d11.h>


void Application::Contents::ContentsLayer::Initialize()
{
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
    yunutyEngine::Collider2D::SetIsOnXYPlane(false);

    //TestColliderEnableDeath();
    TestNavEnableDeath();

    yunutyEngine::YunutyCycle::SingleInstance().Play();
}

void Application::Contents::ContentsLayer::Update(float ts)
{

}

void Application::Contents::ContentsLayer::GUIProgress()
{

}

void Application::Contents::ContentsLayer::Finalize()
{

}
