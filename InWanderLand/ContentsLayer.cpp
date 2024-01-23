#include "InWanderLand.h"
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

std::function<void()> application::contents::ContentsLayer::testInitializer;

/// 그래픽스 테스트용
void GraphicsTest()
{
	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	_resourceManager->CreateMaterial(L"Debug");
	_resourceManager->LoadFile("FBX/Boss");
	_resourceManager->LoadFile("FBX/Bush");
	_resourceManager->LoadFile("FBX/BigTree");
	auto& animationList = _resourceManager->GetAnimationList();

	for (int j = 0; j < 100; ++j)
	{
		float tempX = rand() % 1000;
		float tempZ = rand() % 1000;
		auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Boss");
		object->GetTransform()->position = Vector3d{ tempX,0,tempZ };
		auto animator = object->GetComponent<yunutyEngine::graphics::Animator>();
		for (auto& i : animationList)
		{
			if (i->GetName() == L"root|000.Idle")
			{
				i->SetLoop(true);
				animator->GetGI().PushAnimation(i);
				animator->GetGI().Play(i);
			}

			if (i->GetName() == L"root|001-2.Walk")
			{
				i->SetLoop(true);
				animator->GetGI().PushAnimation(i);
			}
		}
	}

	//for (int j = 0; j < 1; ++j)
	//{
	//	auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Boss");

	//	auto animator = object->GetComponent<yunutyEngine::graphics::Animator>();
	//	for (auto& i : animationList)
	//	{
	//		if (i->GetName() == L"root|000.Idle")
	//		{
	//			i->SetLoop(true);
	//			animator->GetGI().PushAnimation(i);
	//			
	//		}

	//		if (i->GetName() == L"root|001-2.Walk")
	//		{
	//			i->SetLoop(true);
	//			animator->GetGI().PushAnimation(i);
	//			animator->GetGI().Play(i);
	//		}
	//	}
	//}

	//for (int i = 0; i < 500; ++i)
	//{
	//	float tempX = static_cast<float>(rand() % 100);
	//	float tempZ = static_cast<float>(rand() % 100);
	//	auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Bush");
	//	object->GetTransform()->position = Vector3d{ tempX,0,tempZ };
	//	object->GetTransform()->rotation = Quaternion{ Vector3d{90,0,0} };
	//	auto object1 = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("BigTree");
	//	object1->GetTransform()->position = Vector3d{ tempZ,0,tempX };
	//	object1->GetTransform()->rotation = Quaternion{ Vector3d{90,0,0} };
	//}
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
void application::contents::ContentsLayer::Initialize()
{
	if (ContentsLayer::testInitializer)
	{
		ContentsLayer::testInitializer();
		return;
	}

	//auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//camObj->GetTransform()->position = Vector3d(0, 0, -5);
	//auto roamingCam = camObj->AddComponent<RoamingCam>();
#ifdef GRAPHICS_TEST
	{
		yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
		yunutyEngine::Collider2D::SetIsOnXYPlane(false);
		auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
		directionalLight->GetTransform()->rotation = Quaternion{ Vector3d{90,0,45} };
	}
	GraphicsTest();
#else

#endif

	yunutyEngine::YunutyCycle::SingleInstance().Play();
	}

void application::contents::ContentsLayer::Update(float ts)
{
	//std::cout << Time::GetFPS() << std::endl;
}

void application::contents::ContentsLayer::GUIProgress()
{

}

void application::contents::ContentsLayer::Finalize()
{

}

#ifdef GEN_TESTS
void application::contents::ContentsLayer::AssignTestInitializer(std::function<void()> testInitializer)
{
	ContentsLayer::testInitializer = testInitializer;
	YunutyCycle::SingleInstance().onExceptionThrown = [](const std::exception& e) {
		application::Application::GetInstance().AddMainLoopTodo([=]() {
			Assert::Fail(yunutyEngine::yutility::GetWString(e.what()).c_str());
			});
		};
}
#endif
