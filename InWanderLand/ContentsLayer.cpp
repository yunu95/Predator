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

    _resourceManager->LoadFile("FBX/Sponza");
    _resourceManager->LoadFile("FBX/Bush");
    _resourceManager->LoadFile("FBX/Boss");
    _resourceManager->LoadFile("FBX/BigTree");
    _resourceManager->LoadFile("Texture/zoro.jpg");

    auto object3 = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto test = object3->AddComponent<TestComponent2>();
    yunuGI::IAnimation* anim1 = nullptr;
    yunuGI::IAnimation* anim2 = nullptr;
    yunuGI::IAnimation* anim3 = nullptr;
    yunuGI::IMaterial* tempMaterial = nullptr;
    yunuGI::ITexture* tempTexture = nullptr;

    auto& texVec = _resourceManager->GetTextureList();

    for (auto& i : texVec)
    {
        if (i->GetName() == L"Texture/zoro.jpg")
        {
            tempTexture = i;
        }
    }

    for (int j = 0; j < 1; ++j)
    {
        auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Boss");
        auto animator = object->GetComponent<yunutyEngine::graphics::Animator>();
        auto& animationList = _resourceManager->GetAnimationList();
        auto& childVec = object->GetChildren();
        auto renderer = childVec[0]->GetComponent<yunutyEngine::graphics::SkinnedMesh>();
        test->renderer = renderer;
        tempMaterial = _resourceManager->CloneMaterial(L"TempMaterial", renderer->GetGI().GetMaterial(0));
        tempMaterial->SetTexture(yunuGI::Texture_Type::ALBEDO, tempTexture);
        for (auto& i : animationList)
        {
            if (i->GetName() == L"root|000.Idle")
            {
                i->SetLoop(true);
                animator->GetGI().PushAnimation(i);
                anim1 = i;
            }

            if (i->GetName() == L"root|001-2.Walk")
            {
                i->SetLoop(true);
                animator->GetGI().PushAnimation(i);
                anim2 = i;
            }

            if (i->GetName() == L"root|002-1.NormalAttack_R")
            {
                i->SetLoop(true);
                animator->GetGI().PushAnimation(i);
                anim3 = i;
                animator->GetGI().Play(i);
            }
        }
    }
    for (int j = 0; j < 1; ++j)
    {
        auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Boss");
        object->GetTransform()->SetLocalPosition(Vector3d{ 5,0,0 });
        auto animator = object->GetComponent<yunutyEngine::graphics::Animator>();
        auto& animationList = _resourceManager->GetAnimationList();
        for (auto& i : animationList)
        {
            if (i->GetName() == L"root|000.Idle")
            {
                i->SetLoop(true);
                animator->GetGI().PushAnimation(i);
                anim1 = i;
            }

            if (i->GetName() == L"root|001-2.Walk")
            {
                i->SetLoop(true);
                animator->GetGI().PushAnimation(i);
                anim2 = i;
            }

            if (i->GetName() == L"root|002-1.NormalAttack_R")
            {
                i->SetLoop(true);
                animator->GetGI().PushAnimation(i);
                anim3 = i;
                animator->GetGI().Play(i);
            }
        }
    }
    test->material = tempMaterial;


    //auto& shaderList = _resourceManager->GetShaderList();
    //auto& meshList = _resourceManager->GetMeshList();
    //yunuGI::IShader* shader;
    //yunuGI::IMesh* mesh;
    //for (auto& i : shaderList)
    //{
    //	if (i->GetName() == L"DebugPS.cso")
    //	{
    //		shader = i;
    //	}
    //}
    //for (auto& i : meshList)
    //{
    //	if (i->GetName() == L"Cube")
    //	{
    //		mesh = i;
    //	}
    //}
    ////for (int i = 0; i < 1500; ++i)
    ////{
    ////	float tempX = static_cast<float>(rand() % 100);
    ////	float tempZ = static_cast<float>(rand() % 100);
    ////	auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Bush");
    ////	object->GetTransform()->position = Vector3d{ tempX,0,tempZ };
    ////	object->GetTransform()->rotation = Quaternion{ Vector3d{90,0,0} };
    ////	auto object1 = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("BigTree");
    ////	object1->GetTransform()->position = Vector3d{ tempZ,0,tempX };
    ////	object1->GetTransform()->rotation = Quaternion{ Vector3d{90,0,0} };
    ////}

    ////{
    ////	auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Sponza");
    ////	object->GetTransform()->rotation = Quaternion{ Vector3d{90,0,0} };
    ////}

    //auto material = _resourceManager->CreateMaterial(L"TEMP");
    //material->SetPixelShader(shader);

    //auto object1 = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //object1->GetTransform()->position = Vector3d{ -5,0,0};
    //object1->GetTransform()->rotation = Quaternion{ Vector3d{90,0,0} };
    //auto renderer = object1->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    ////renderer->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1,0,0,1 });
    ////renderer->GetGI().SetMaterial(0, material);
    //renderer->GetGI().SetMesh(mesh);

    //auto object2 = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //object2->GetTransform()->position = Vector3d{ 5,0,0};
    //object2->GetTransform()->rotation = Quaternion{ Vector3d{90,0,0} };
    //auto renderer2 = object2->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    ////renderer->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1,0,0,1 });
    ////renderer->GetGI().SetMaterial(0, material);
    //renderer2->GetGI().SetMesh(mesh);

    //auto object3 = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //auto test = object3->AddComponent<TestComponent2>();
    //test->renderer = renderer2;
    //test->material = material;
    //test->shader = shader;


    //{
    //	auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //	object->GetTransform()->position = Vector3d{ 0,0.25,0 };
    //	auto light = object->AddComponent<yunutyEngine::graphics::PointLight>();
    //	yunuGI::Color color = yunuGI::Color{ 0,0,1,1 };
    //	light->GetGI().SetLightDiffuseColor(color);
    //	light->GetGI().SetRange(0.3);
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

    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
    auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    camObj->GetTransform()->SetLocalPosition(Vector3d(0, 20, -10));
    camObj->GetTransform()->SetLocalRotation( Quaternion(Vector3d(60, 0, 0)));
    auto roamingCam = camObj->AddComponent<RTSCam>();
    roamingCam->SetCameraMain();

    const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    resourceManager->LoadFile("FBX/Bush");
    resourceManager->LoadFile("FBX/BigTree");

#ifdef GRAPHICS_TEST
    {
        //yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
        yunutyEngine::Collider2D::SetIsOnXYPlane(false);
        auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto light = directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
        auto color = yunuGI::Color{ 0.831,0.722,0.569,1.f };
        light->GetGI().SetLightDiffuseColor(color);
        directionalLight->GetTransform()->position = Vector3d{ 0,0,-10 };
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
