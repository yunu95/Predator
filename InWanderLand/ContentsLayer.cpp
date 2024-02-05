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
#include "TestUtilGraphicsTestCam.h"

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
    auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    camObj->AddComponent<tests::GraphicsTestCam>();

    const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

    _resourceManager->LoadFile("FBX/Monster2");
	auto& animationList = _resourceManager->GetAnimationList();
	{
		yunuGI::IAnimation* walkAnim = nullptr;
		auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("Monster2");
        obj->GetTransform()->SetLocalScale(Vector3d{ obj->GetTransform()->GetLocalScale().x * 100.f,obj->GetTransform()->GetLocalScale().y * 100.f,obj->GetTransform()->GetLocalScale().z * 100.f });
        auto& childVec = obj->GetChildren();

		auto animator = obj->GetComponent<graphics::Animator>();
		for (auto& i : animationList)
		{
			if (i->GetName() == L"Ani_Monster2_Walk")
			{
				walkAnim = i;
			}
		}
		walkAnim->SetLoop(true);
		animator->GetGI().PushAnimation(walkAnim);
		animator->GetGI().Play(walkAnim);
	}

    _resourceManager->LoadFile("FBX/CastleWall_Door");
	{
		auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("CastleWall_Door");
		obj->GetTransform()->SetLocalPosition(Vector3d{ 0,0,0 });
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
void application::contents::ContentsLayer::Initialize()
{
    if (ContentsLayer::testInitializer)
    {
        ContentsLayer::testInitializer();
        return;
    }

    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
    //auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //camObj->GetTransform()->SetLocalPosition(Vector3d(0, 20, -10));
    //camObj->GetTransform()->SetLocalRotation( Quaternion(Vector3d(60, 0, 0)));
    //auto roamingCam = camObj->AddComponent<RTSCam>();
    //roamingCam->SetCameraMain();
#ifdef GRAPHICS_TEST
    {
        //yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
        yunutyEngine::Collider2D::SetIsOnXYPlane(false);
        auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto light = directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
        auto color = yunuGI::Color{ 0.831,0.722,0.569,1.f };
        light->GetGI().SetLightDiffuseColor(color);
        directionalLight->GetTransform()->SetLocalPosition(Vector3d{ 0,0,-10 });
        //directionalLight->GetTransform()->rotation = Quaternion{ Vector3d{0, 45, 0} };
        //auto test = directionalLight->AddComponent<TestComponent2>();
        //test->gameObject = directionalLight;
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
