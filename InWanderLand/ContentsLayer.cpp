#include "InWanderLand.h"
#include "ContentsLayer.h"

#include "YunutyEngine.h"
#include "ShakyCam.h"
#include "RTSCam.h"   
#include "DebugBeacon.h"
#include "DebugMeshes.h"
#include "Application.h"
#include "TestComponent2.h"
#include "TestUtilGraphicsTestCam.h"
#include "MapFileManager.h"
#include "InstanceManager.h"
#include "SingleNavigationField.h"
#include "TestUtilRTSTestCam.h"
#include "WarriorProductor.h"
#include "MagicianProductor.h"
#include "HealerProductor.h"
#include "BossProductor.h"
#include "EnemySummonGateProductor.h"
#include "InputManager.h"
#include "UIManager.h"
#include "PlayerController.h"
#include "TacticModeSystem.h"
#include "GameManager.h"
#include "SingletonInstanceContainer.h"
#include "ShortcutSystem.h"
#include "RobinSkillDevelopmentSystem.h"

#include <algorithm>
#include <string>
#include <filesystem>
#include "WanderUtils.h"

#ifdef GEN_TESTS
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
#endif

#include <d3d11.h>

std::function<void()> application::contents::ContentsLayer::testInitializer;

bool contentsInputControl = true;

/// 그래픽스 테스트용
void GraphicsTest()
{
    auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    camObj->AddComponent<tests::GraphicsTestCam>();

    const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

    _resourceManager->LoadFile("Texture/Brick_Albedo.jpg");
    _resourceManager->LoadFile("Texture/Brick_Normal.jpg");

    yunuGI::IMesh* mesh = _resourceManager->GetMesh(L"Cube");
    yunuGI::IMesh* mesh2 = _resourceManager->GetMesh(L"Sphere");
    yunuGI::IMesh* mesh3 = _resourceManager->GetMesh(L"Rectangle");
    yunuGI::ITexture* tex = _resourceManager->GetTexture(L"Texture/Brick_Albedo.jpg");
    yunuGI::ITexture* tex2 = _resourceManager->GetTexture(L"Texture/Brick_Normal.jpg");

    auto& animationList = _resourceManager->GetAnimationList();
    yunuGI::IAnimation* animation;

    for (auto& i : animationList)
    {
        if (i->GetName() == L"Ani_Monster2_Walk")
        {
            i->SetLoop(true);
            animation = i;
            int a = 1;
            i->SetEventFunc(20, []() {std::cout << "TEST" << std::endl; });
        }
    }

    auto& shaderList = _resourceManager->GetShaderList();
    yunuGI::IShader* shader;
    yunuGI::IShader* shader2;
    for (auto& i : shaderList)
    {
        if (i->GetName() == L"LeavesVS.cso")
        {
            shader = i;
        }

        if (i->GetName() == L"LeavesPS.cso")
        {
            shader2 = i;
        }
    }

    {
        auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        //obj->GetTransform()->SetLocalPosition(Vector3d{ -500,500,1 });
        obj->GetTransform()->SetLocalScale(Vector3d{ 100,100,100 });
        auto text = obj->AddComponent<yunutyEngine::graphics::UIText>();
        text->GetGI().SetFontSize(20);
        auto test = obj->AddComponent<TestComponent2>();
        test->text = text;
    }

    /*   {
           auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
           obj->GetTransform()->SetLocalPosition(Vector3d{ -47.56,3.67,44.81 });
           auto light = obj->AddComponent<yunutyEngine::graphics::PointLight>();
           yunuGI::Color color{ 0,0,1.f,1 };
           light->GetGI().SetLightDiffuseColor(color);
           light->GetGI().SetRange(10);
       }

       {
           auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
           obj->GetTransform()->SetLocalPosition(Vector3d{ -47.56,3.67,45.32 });
           auto light = obj->AddComponent<yunutyEngine::graphics::PointLight>();
           yunuGI::Color color{ 1.f,0,0,1 };
           light->GetGI().SetLightDiffuseColor(color);
           light->GetGI().SetRange(10);
       }

       {
           auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
           obj->GetTransform()->SetLocalScale(Vector3d{ 100,1,100 });
           auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
           renderer->GetGI().SetMesh(mesh);
       }
       {
           auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
           obj->GetTransform()->SetLocalScale(Vector3d{ 100,100,1 });
           obj->GetTransform()->SetLocalPosition(Vector3d{ 0,50,50 });
           auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
           renderer->GetGI().SetMesh(mesh);
       }
       {
           auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
           obj->GetTransform()->SetLocalScale(Vector3d{ 1,100,100 });
           obj->GetTransform()->SetLocalPosition(Vector3d{ -50,50,0 });
           auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
           renderer->GetGI().SetMesh(mesh);
       }

       {
           auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
           obj->GetTransform()->SetLocalPosition(Vector3d{ -48.67,3.67,44.81 });
           auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
           renderer->GetGI().SetMesh(mesh);
       }
       {
           auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
           obj->GetTransform()->SetLocalPosition(Vector3d{ -47.26,3.67,47.83 });
           auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
           renderer->GetGI().SetMesh(mesh);
       }
       {
           auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
           obj->GetTransform()->SetLocalPosition(Vector3d{ -48.08,5.32,46.24 });
           auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
           renderer->GetGI().SetMesh(mesh);
       }

       {
           auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
           obj->GetTransform()->SetLocalScale(Vector3d{ 1,100,100 });
           obj->GetTransform()->SetLocalPosition(Vector3d{ 48.08,5.32,46.24 });
           auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
           renderer->GetGI().SetMesh(mesh);
       }

       {
           auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Monster2");
           auto animator = obj->GetComponent<yunutyEngine::graphics::Animator>();
           animator->GetGI().PushAnimation(animation);
           animator->GetGI().Play(animation);
           obj->GetTransform()->SetLocalPosition(Vector3d{ -47.55, 0.5f,42.53 });
       }


       {
           auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
           auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
           renderer->GetGI().SetMesh(mesh);
       }*/
       ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

       //{
       //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Monster1");
       //}
       //{
       //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Robin");
       //	obj->GetTransform()->SetLocalPosition(Vector3d{ 10,0,0 });
       //}
       //{
       //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Bush_001");
       //	obj->GetTransform()->SetLocalPosition(Vector3d{ 10,0,0 });
       //}
       //{
       //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Bush_001");
       //	obj->GetTransform()->SetLocalScale(Vector3d{ 2,2,2 });
       //}
       //{
       //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Stump");
       //	obj->GetTransform()->SetLocalPosition(Vector3d{ 50,0,0 });
       //}

       ////////////{
       ////////////	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Robin");
       ////////////	obj->GetTransform()->SetLocalPosition(Vector3d{ 0,0,0 });
       ////////////}
       ////////////{
       ////////////	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_CastleWall_Door");
       ////////////	obj->GetTransform()->SetLocalPosition(Vector3d{ 50,0,0 });
       ////////////	obj->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{0,180,0} });
       ////////////}
       ////////////{
       ////////////	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Cuptower");
       ////////////	obj->GetTransform()->SetLocalPosition(Vector3d{ 70,0,0 });
       ////////////}
       ////////////{
       ////////////	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Fork");
       ////////////	obj->GetTransform()->SetLocalPosition(Vector3d{ 90,0,0 });
       ////////////}
       ////////////{
       ////////////	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Hat01");
       ////////////	obj->GetTransform()->SetLocalPosition(Vector3d{ 110,0,0 });
       ////////////}
       ////////////{
       ////////////	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Hat02");
       ////////////	obj->GetTransform()->SetLocalPosition(Vector3d{ 130,0,0 });
       ////////////}
       ////////////{
       ////////////	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Stump");
       ////////////	obj->GetTransform()->SetLocalPosition(Vector3d{ 150,0,0 });
       ////////////}
       ////////////{
       ////////////	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Mushroom01");
       ////////////	obj->GetTransform()->SetLocalPosition(Vector3d{ 170,0,0 });
       ////////////}
       ////////////{
       ////////////	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Mushroom02");
       ////////////	obj->GetTransform()->SetLocalPosition(Vector3d{ 190,0,0 });
       ////////////}

    {
        auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Bush_001");
    }

    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Monster1");
    //	obj->GetTransform()->SetLocalPosition({ Vector3d{5,0,-5} });
    //}
    //
    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Monster2");
    //	obj->GetTransform()->SetLocalPosition({ Vector3d{10,0,-5} });
    //}
    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Cuptower");
    //	obj->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{0,180,0} });
    //}

  /*  {
        auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_CastleWall");
        auto renderer = obj->GetChildren()[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        for (int i = 0; i < renderer->GetGI().GetMaterialCount(); ++i)
        {
            renderer->GetGI().GetMaterial(i)->SetPixelShader(_resourceManager->GetShader(L"Debug_AlphaPS.cso"));
            yunuGI::Color color{ 1,0,0,0.2 };
            renderer->GetGI().GetMaterial(i)->SetColor(color);
        }
    }

    {
        auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        obj->GetTransform()->SetLocalPosition(Vector3d{ 0,0,0 });
        auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        renderer->GetGI().SetMesh(mesh);
        renderer->GetGI().GetMaterial()->SetPixelShader(_resourceManager->GetShader(L"Debug_AlphaPS.cso"));
        yunuGI::Color color{ 1,0,0,0.1 };
        renderer->GetGI().GetMaterial()->SetColor(color);
    }

    {
        auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        obj->GetTransform()->SetLocalPosition(Vector3d{ 0,0,0 });
        auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        renderer->GetGI().SetMesh(mesh2);
    }*/
}

void application::contents::ContentsLayer::SetInputControl(bool control)
{
    contentsInputControl = control;
}

bool application::contents::ContentsLayer::GetInputControl()
{
    return contentsInputControl;
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
    ShortcutInit();

    wanderUtils::LoadResourcesRecursively();

#ifndef EDITOR
#ifdef GRAPHICS_TEST
    {
        yunutyEngine::Collider2D::SetIsOnXYPlane(false);
        auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        directionalLight->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{90,0,0} });
        directionalLight->GetTransform()->SetLocalPosition(Vector3d{ 0,0,-20 });
        auto light = directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
        auto color = yunuGI::Color{ 1,1,1,1.f };
        light->GetGI().SetLightDiffuseColor(color);

        //editor::MapFileManager::GetSingletonInstance().LoadStaticOrnaments("TestOrnaments.punreal");
    }
    GraphicsTest();
#else
    {

        ShortcutSystem::Instance();

        {
            auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
            //obj->GetTransform()->SetLocalPosition(Vector3d{ -500,500,1 });
            obj->GetTransform()->SetLocalScale(Vector3d{ 100,100,100 });
            auto text = obj->AddComponent<yunutyEngine::graphics::UIText>();
            text->GetGI().SetFontSize(20);
            auto test = obj->AddComponent<TestComponent2>();
            test->text = text;
        }

        auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

        auto sphereMesh = rsrcMgr->GetMesh(L"Sphere");
        auto mouseCursorObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        RegisterToEditorObjectVector(mouseCursorObject);
        auto mouseCursorMesh = mouseCursorObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        mouseCursorMesh->GetGI().SetMesh(sphereMesh);
        mouseCursorMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 0, 1 });

        /*WarriorProductor::Instance().CreateUnit(Vector3d(0.0f, 0.0f, 0.0f));;
        MagicianProductor::Instance().CreateUnit(Vector3d(0.0f, 0.0f, 2.0f));;
        HealerProductor::Instance().CreateUnit(Vector3d(0.0f, 0.0f, -2.0f));*/

        auto skillPreviewCubeMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        AttachDebugMesh(skillPreviewCubeMeshObject, DebugMeshType::Cube)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
        SkillPreviewSystem::Instance().SetPathPreviewObject(skillPreviewCubeMeshObject);
        RegisterToEditorObjectVector(skillPreviewCubeMeshObject);

        auto skillPreviewSphereMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        AttachDebugMesh(skillPreviewSphereMeshObject, DebugMeshType::Sphere)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
        SkillPreviewSystem::Instance().SetRangePreviewObject(skillPreviewSphereMeshObject);
        RegisterToEditorObjectVector(skillPreviewSphereMeshObject);

        InputManager::Instance();
        UIManager::Instance();
        bool mapFound = false;


        editor::MapFileManager::GetSingletonInstance().LoadMapFile("InWanderLand.pmap");
        editor::InstanceManager::GetSingletonInstance().ApplyInstancesAsPlaytimeObjects();

    }
#endif
#else
    // EDITOR
    /*if (std::filesystem::exists("InWanderLand.pmap"))
    {
        editor::MapFileManager::GetSingletonInstance().LoadMapFile("InWanderLand.pmap");
    }*/
#endif

}

void application::contents::ContentsLayer::Update(float ts)
{
    std::cout << Time::GetFPS() << std::endl;
}

void application::contents::ContentsLayer::GUIProgress()
{

}

void application::contents::ContentsLayer::Finalize()
{

}

void application::contents::ContentsLayer::PlayContents()
{
    SingletonInstanceContainer::SingleInstance().PermitCreateInstances();
    editor::InstanceManager::GetSingletonInstance().ApplyInstancesAsPlaytimeObjects();
    GameManager::Instance().Reset();
    for (auto e : componentsCreatedByEditorVector)
    {
        e->SetActive(true);
    }

    //InputManager::Instance();
    //UIManager::Instance();

    /// Editor 에서 수정하여 Map Data 에 저장할 부분

    /// 카메라의 경우 CameraData 의 ApplyInstancesAsPlaytimeObjects 에서 처리함

    auto skillPreviewCubeMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    AttachDebugMesh(skillPreviewCubeMeshObject, DebugMeshType::Cube)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
    SkillPreviewSystem::Instance().SetPathPreviewObject(skillPreviewCubeMeshObject);

    auto skillPreviewSphereMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    AttachDebugMesh(skillPreviewSphereMeshObject, DebugMeshType::Sphere)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
    SkillPreviewSystem::Instance().SetRangePreviewObject(skillPreviewSphereMeshObject);

    /// 임시
    //RegisterToEditorObjectVector(MagicianProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, -7.0f))->GetGameObject());
    //RegisterToEditorObjectVector(HealerProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, 7.0f))->GetGameObject());
    //RegisterToEditorObjectVector(BossProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, 7.0f))->GetGameObject());
    //RegisterToEditorObjectVector(BossProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, 7.0f))->GetGameObject());

    /// UI 작업
    const int menuWindowXpos = 760;
    const int menuWindowYpos = 540 - 350 / 2;

    auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    rsrcMgr->LoadFile("Texture/zoro.jpg");
    rsrcMgr->LoadFile("Texture/zoro_highLighted.jpg");
    rsrcMgr->LoadFile("Texture/zoro_Clicked.jpg");

    /*  auto robinPassiveActivateButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
      auto robinPassiveActivateImage = robinPassiveActivateButtonObject->AddComponent<yunutyEngine::graphics::UIImage>();
      auto robinPassiveActivateButton = robinPassiveActivateButtonObject->AddComponent<UIButton>();
      robinPassiveActivateButton->SetImageComponent(robinPassiveActivateImage);
      robinPassiveActivateButton->SetIdleImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
      robinPassiveActivateButton->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/zoro_highLighted.jpg"));
      robinPassiveActivateButton->SetClickedImage(rsrcMgr->GetTexture(L"Texture/zoro_Clicked.jpg"));
      robinPassiveActivateButton->SetLayer(10.0);
      RobinSkillDevelopmentSystem::Instance().AddTopLayerButton(robinPassiveActivateButton);
      robinPassiveActivateButtonObject->GetTransform()->SetLocalPosition({ 0, 700, 0 });
      RegisterToEditorObjectVector(robinPassiveActivateButtonObject);

      auto robinWSkillUpgradeButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
      auto robinWSkillUpgradeImage = robinWSkillUpgradeButtonObject->AddComponent<yunutyEngine::graphics::UIImage>();
      auto robinWSkillUpgradeButton = robinWSkillUpgradeButtonObject->AddComponent<UIButton>();
      robinWSkillUpgradeButton->SetImageComponent(robinWSkillUpgradeImage);
      robinWSkillUpgradeButton->SetIdleImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
      robinWSkillUpgradeButton->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
      robinWSkillUpgradeButton->SetClickedImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
      robinWSkillUpgradeButton->SetLayer(10.0);
      RobinSkillDevelopmentSystem::Instance().AddMiddleLayerButton(robinWSkillUpgradeButton);
      robinWSkillUpgradeButtonObject->GetTransform()->SetLocalPosition({ 100, 700, 0 });
      RegisterToEditorObjectVector(robinWSkillUpgradeButtonObject);*/

}

void application::contents::ContentsLayer::PauseContents()
{
    Time::SetTimeScale(FLT_MIN * 1000);
    for (auto e : componentsCreatedByEditorVector)
    {
        e->SetActive(false);
    }
}

void application::contents::ContentsLayer::ResumeContents()
{
    Time::SetTimeScale(1);
    for (auto e : componentsCreatedByEditorVector)
    {
        e->SetActive(true);
    }
}

void application::contents::ContentsLayer::StopContents()
{
    Time::SetTimeScale(1);
    isStoppedOnce = true;
    ClearPlaytimeObject();
    ShortcutSystem::Instance().Clear();
    for (auto e : componentsCreatedByEditorVector)
    {
        e->SetActive(false);
    }
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

void application::contents::ContentsLayer::ClearPlaytimeObject()
{
    /// 생성된 모든 게임 플레이 오브젝트 삭제
    for (auto e : objectCreatedByEditorVector)
    {
        //      if (e->getName() == "")
              //e->SetSelfActive(false);
        yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(e);
    }

    for (auto e : componentsCreatedByEditorVector)
    {
        e->SetActive(false);
    }
    objectCreatedByEditorVector.clear();

    SingletonInstanceContainer::SingleInstance().ClearSingletonInstances();
}

void application::contents::ContentsLayer::ShortcutInit()
{
    auto& scsys = ShortcutSystem::Instance();
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::D, false } },
        [=]()
        {
            DebugGraphic::SetDebugGraphicsEnabled(!DebugGraphic::AreDebugGraphicsEnabled());
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::NUM_1, false } },
        [=]()
        {
            for (auto& each : yunutyEngine::Scene::getCurrentScene()->GetChildren())
            {
                auto comp = each->GetComponent<Unit>();
                if (comp == nullptr)
                {
                    continue;
                }

                if (comp->GetUnitSide() == Unit::UnitSide::Player)
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    comp->GetGameObject()->SetSelfActive(scsysIns.GetTriggerSwitch(scsysIns.GetKeyIndex({ { KeyCode::Control, true }, { KeyCode::NUM_1, false } })));
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::NUM_2, false } },
        [=]()
        {
            for (auto& each : yunutyEngine::Scene::getCurrentScene()->GetChildren())
            {
                auto comp = each->GetComponent<Unit>();
                if (comp == nullptr)
                {
                    continue;
                }

                if (comp->GetUnitSide() == Unit::UnitSide::Enemy)
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    comp->GetGameObject()->SetSelfActive(scsysIns.GetTriggerSwitch(scsysIns.GetKeyIndex({ { KeyCode::Control, true }, { KeyCode::NUM_2, false } })));
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::NUM_3, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                auto& scsysIns = ShortcutSystem::Instance();
                each->GetPaletteInstance()->GetGameObject()->SetSelfActive(scsysIns.GetTriggerSwitch(scsysIns.GetKeyIndex({ { KeyCode::Control, true }, { KeyCode::NUM_3, false } })));
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::NUM_4, false } },
        [=]()
        {
            for (auto& each : yunutyEngine::Scene::getCurrentScene()->GetChildren())
            {
                auto comp = each->GetComponent<yunutyEngine::graphics::PointLight>();
                if (comp == nullptr)
                {
                    continue;
                }

                auto& scsysIns = ShortcutSystem::Instance();
                comp->GetGameObject()->SetSelfActive(scsysIns.GetTriggerSwitch(scsysIns.GetKeyIndex({ { KeyCode::Control, true }, { KeyCode::NUM_4, false } })));
            }
        });

    /// Ornament 개별 적용
#pragma region
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_1, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Bush_001")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_2, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Bush_002")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_3, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Stone_001")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_4, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Stone_002")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_5, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Grass_001")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_6, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Grass_002")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_7, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_LeafShadow")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_8, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Pebbles_001")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_9, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Book01")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_0, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Book03")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Q, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Book05")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::W, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Book06")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alphabet_E, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Bookshelf01")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::R, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Bookshelf02")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::T, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Bookshelf03")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Y, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Bookshelf04")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::U, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Mushroom01")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::I, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Mushroom02")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::O, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_BigTree_001")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::P, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_BossCorrider_Floor")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::A, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Candle")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::S, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_CastleWall")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::D, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_CastleWall_Door")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::F, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_CastleWall_Pillar")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::G, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Chair")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::H, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Corrider_Floor")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::J, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Corrider_Floor06")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::K, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_CupTower")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::L, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Entrance_Floor")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Z, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_FloorBrick")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::X, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Fork")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::C, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_GuideBook")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::V, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Hall_Corrider_Floor")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::B, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Hall_Entrance_Floor")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::N, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Hall_Floor")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::M, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Hat01")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_1, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Hat02")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_2, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Key_Room_Floor")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_3, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Lamp")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_4, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Pillar")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_5, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Room_Floor01")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_6, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_SmallBush_001")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_7, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Stage1_Floor")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_8, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Stump")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_9, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Book_etc")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_0, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Books")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::Q, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Floor")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::W, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Pillar")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::Alphabet_E, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Pillar_Broken")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::R, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Rabbit")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::T, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Welcome")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::Y, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Trunk_001")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::U, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_VertexColor")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::I, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Wall_3m")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::O, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Wall_7m")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::P, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Wall_21m")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::A, false } },
        [=]()
        {
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
            {
                if (each->pod.templateData->pod.staticFBXName == "SM_Arch_Window_7m")
                {
                    auto& scsysIns = ShortcutSystem::Instance();
                    each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
                }
            }
        });
#pragma endregion Ornament Shortcut
}

void application::contents::ContentsLayer::RegisterToEditorObjectVector(GameObject* p_obj)
{
    objectCreatedByEditorVector.push_back(p_obj);
}

void application::contents::ContentsLayer::RegisterToEditorComponentVector(Component* p_com)
{
    componentsCreatedByEditorVector.push_back(p_com);
}

