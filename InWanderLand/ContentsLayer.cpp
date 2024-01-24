#include "ContentsLayer.h"

#include "YunutyEngine.h"
#include "ShakyCam.h"
#include "RTSCam.h"   
#include "DebugTilePlane.h"
#include "DebugBeacon.h"
#include "DebugMeshes.h"
#include "Application.h"
#include "TestComponent2.h"

#include <algorithm>
#include <string>
#ifdef GEN_TESTS
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
#endif

#include <d3d11.h>

std::function<void()> application::Contents::ContentsLayer::testInitializer;

/// 그래픽스 테스트용
void GraphicsTest()
{
	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	//_resourceManager->LoadFile("FBX/Bush");
	_resourceManager->LoadFile("FBX/Stone");
	_resourceManager->LoadFile("Texture/T_LeafBrush.png");
	_resourceManager->LoadFile("LeavesPS.cso");
	_resourceManager->LoadFile("LeavesVS.cso");
	auto& meshList = _resourceManager->GetMeshList();
	auto& shaderList = _resourceManager->GetShaderList();
	auto& textureList = _resourceManager->GetTextureList();

	yunuGI::IMesh* planeMesh = nullptr;
	yunuGI::IMesh* sphereMesh = nullptr;
	yunuGI::IMesh* cubeMesh = nullptr;
	yunuGI::IShader* pshader = nullptr;
	yunuGI::IShader* vshader = nullptr;
	yunuGI::ITexture* texture = nullptr;

	auto material = _resourceManager->CreateMaterial(L"Leaves");

	for (auto& i : shaderList)
	{
		if (i->GetName() == L"LeavesPS.cso")
		{
			pshader = i;
		}
		if (i->GetName() == L"LeavesVS.cso")
		{
			vshader = i;
		}
	}

	for (auto& i : meshList)
	{
		if (i->GetName() == L"Rectangle")
		{
			planeMesh = i;
		}
		if (i->GetName() == L"Sphere")
		{
			sphereMesh = i;
		}
		if (i->GetName() == L"Cube")
		{
			cubeMesh = i;
		}
	}

	for (auto& i : textureList)
	{
		if (i->GetName() == L"Texture/T_LeafBrush.png")
		{
			texture = i;
		}
	}

	{
		auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Stone");
		obj->GetTransform()->rotation = Quaternion{ Vector3d{90,0,0} };
	}
}


//void CreateNavPlane(Vector3f botleft, Vector3f topright, std::vector<Vector3f>& worldVertices, std::vector<int>& worldFaces)
//{
//    int startingIdx = worldVertices.size();
//    worldVertices.push_back({ botleft.x,0,topright.z });
//    worldVertices.push_back({ botleft.x,0,botleft.z });
//    worldVertices.push_back({ topright.x,0,botleft.z });
//    worldVertices.push_back({ topright.x,0,topright.z });
//
//    worldFaces.push_back(startingIdx + 2);
//    worldFaces.push_back(startingIdx + 1);
//    worldFaces.push_back(startingIdx + 0);
//    worldFaces.push_back(startingIdx + 3);
//    worldFaces.push_back(startingIdx + 2);
//    worldFaces.push_back(startingIdx + 0);
//
//    auto tilePlane = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DebugTilePlane>();
//    auto size = topright - botleft;
//    tilePlane->GetTransform()->SetWorldPosition((botleft + topright) / 2.0);
//    tilePlane->width = size.x;
//    tilePlane->height = size.z;
//    tilePlane->SetTiles();
//}
//
//NavigationAgent* CreateAgent(NavigationField* navField)
//{
//    auto agent = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationAgent>();
//    agent->SetSpeed(2);
//    agent->SetRadius(0.5);
//    agent->AssignToNavigationField(navField);
//    auto staticMesh = agent->GetGameObject()->AddGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
//
//    const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
//    auto& meshList = _resourceManager->GetMeshList();
//    for (auto& e : meshList)
//    {
//        if (e->GetName() == L"Capsule")
//        {
//            staticMesh->GetGI().SetMesh(e);
//        }
//    }
//
//    staticMesh->GetGI().GetMaterial()->SetColor({ 0.75,0.75,0.75,0 });
//    staticMesh->GetTransform()->position = Vector3d{ 0,0.5,0 };
//    return agent;
//}
void application::Contents::ContentsLayer::Initialize()
{
	if (ContentsLayer::testInitializer)
	{
		ContentsLayer::testInitializer();
		return;
	}
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
	yunutyEngine::Collider2D::SetIsOnXYPlane(false);

	//auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//camObj->GetTransform()->position = Vector3d(0, 0, -5);
	//auto roamingCam = camObj->AddComponent<RoamingCam>();

	GraphicsTest();

	{
		auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto light =directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
		auto color = yunuGI::Color{ 0.831,0.722,0.569,1.f };
		light->GetGI().SetLightDiffuseColor(color);
		directionalLight->GetTransform()->position = Vector3d{ 0,0,-10 };
		//directionalLight->GetTransform()->rotation = Quaternion{ Vector3d{0, 45, 0} };
		//auto test = directionalLight->AddComponent<TestComponent2>();
		//test->gameObject = directionalLight;
	}

	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();

	auto rtsCam = camObj->AddComponent<RTSCam>();
	//auto rtsCam = camObj->AddComponent<yunutyEngine::graphics::Camera>();
	//rtsCam->GetTransform()->position = Vector3d(0, 50, -450);

	//// 길찾기 테스트
	//{
	//    const float corridorRadius = 3;
	//    std::vector<Vector3f> worldVertices { };
	//    std::vector<int> worldFaces { };

	//    CreateNavPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
	//    CreateNavPlane({ -8,0,-2 }, { 8,0,2 }, worldVertices, worldFaces);
	//    CreateNavPlane({ -8,0,-8 }, { -6,0,8 }, worldVertices, worldFaces);
	//    CreateNavPlane({ 6,0,-8 }, { 8,0,8 }, worldVertices, worldFaces);
	//    CreateNavPlane({ -8,0,6 }, { 8,0,8 }, worldVertices, worldFaces);
	//    CreateNavPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
	//    auto navField = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationField>();
	//    navField->BuildField(worldVertices, worldFaces);
	//    auto agent = CreateAgent(navField);
	//    auto agent2 = CreateAgent(navField);
	//    auto agent3 = CreateAgent(navField);
	//    rtsCam->groundRightClickCallback = [=](Vector3d position) {
	//        agent->MoveTo(position);
	//        agent2->MoveTo(position);
	//        agent3->MoveTo(position);
	//    };
	//}

	yunutyEngine::YunutyCycle::SingleInstance().Play();
}

void application::Contents::ContentsLayer::Update(float ts)
{
	std::cout << Time::GetFPS() << std::endl;
}

void application::Contents::ContentsLayer::GUIProgress()
{

}

void application::Contents::ContentsLayer::Finalize()
{

}

#ifdef GEN_TESTS
void application::Contents::ContentsLayer::AssignTestInitializer(std::function<void()> testInitializer)
{
	ContentsLayer::testInitializer = testInitializer;
	YunutyCycle::SingleInstance().onExceptionThrown = [](const std::exception& e) {
		application::Application::GetInstance().AddMainLoopTodo([=]() {
			Assert::Fail(yunutyEngine::yutility::GetWString(e.what()).c_str());
			});
	};
}
#endif
