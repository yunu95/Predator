#include "ContentsLayer.h"

#include "YunutyEngine.h"
#include "ShakyCam.h"

#include <d3d11.h>

void Application::Contents::ContentsLayer::Initialize()
{
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	camObj->AddComponent<ShakyCam>()->SetCameraMain();

	auto staticMeshObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto staticMesh = staticMeshObj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	staticMesh->GetGI().LoadMesh("Cube");
	///staticMesh->GetGI().GetMaterial()->SetPixelShader(L"DebugPS.cso");
	staticMesh->GetGI().GetMaterial()->SetTexture(yunuGI::Texture_Type::ALBEDO, L"Texture/zoro.jpg");
	staticMeshObj->GetTransform()->SetWorldPosition(yunutyEngine::Vector3d{ 1,3,10 });
	//staticMesh->GetGI().LoadDiffuseMap("Textures/000000002405_reverse.dds");
	//staticMesh->GetGI().LoadNormalMap("Textures/000000002406_b_reverse.dds");

	yunutyEngine::YunutyCycle::SingleInstance().autoRendering = false;
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
