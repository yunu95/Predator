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
#include "ScriptSystem.h"
#include "Script.h"
#include "UIImage.h"
#include "CinematicManager.h"
#include "PlayableComponent.h"

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

    auto& animationList = _resourceManager->GetAnimationList();
    yunuGI::IAnimation* animation;

    for (auto& i : animationList)
    {
        if (i->GetName() == L"Ani_Monster1_Skill")
        {
            i->SetLoop(true);
            animation = i;
        }
    }

    {
        auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Monster1");
        auto animator = obj->GetComponent<yunutyEngine::graphics::Animator>();
        animator->GetGI().PushAnimation(animation);
        animator->GetGI().Play(animation);
    }
	{
		auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Monster1");
        obj->GetTransform()->SetLocalPosition(Vector3d{ 5,0,0 });
		auto animator = obj->GetComponent<yunutyEngine::graphics::Animator>();
		animator->GetGI().PushAnimation(animation);
		animator->GetGI().Play(animation);
	}

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
    ScriptSystem::Instance();
    CinematicManager::Instance();

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

        editor::MapFileManager::GetSingletonInstance().LoadMapFile("InWanderLand.pmap");
        ContentsLayer::PlayContents();

        //ShortcutSystem::Instance();

        //{
        //    auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        //    //obj->GetTransform()->SetLocalPosition(Vector3d{ -500,500,1 });
        //    obj->GetTransform()->SetLocalScale(Vector3d{ 100,100,100 });
        //    auto text = obj->AddComponent<yunutyEngine::graphics::UIText>();
        //    text->GetGI().SetFontSize(20);
        //    auto test = obj->AddComponent<TestComponent2>();
        //    test->text = text;
        //}

        //auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

        //auto sphereMesh = rsrcMgr->GetMesh(L"Sphere");
        //auto mouseCursorObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        //RegisterToEditorObjectContainer(mouseCursorObject);
        //auto mouseCursorMesh = mouseCursorObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        //mouseCursorMesh->GetGI().SetMesh(sphereMesh);
        //mouseCursorMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 0, 1 });

        ///*WarriorProductor::Instance().CreateUnit(Vector3d(0.0f, 0.0f, 0.0f));;
        //MagicianProductor::Instance().CreateUnit(Vector3d(0.0f, 0.0f, 2.0f));;
        //HealerProductor::Instance().CreateUnit(Vector3d(0.0f, 0.0f, -2.0f));*/

        //auto skillPreviewCubeMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        //AttachDebugMesh(skillPreviewCubeMeshObject, DebugMeshType::Cube)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
        //SkillPreviewSystem::Instance().SetPathPreviewObject(skillPreviewCubeMeshObject);
        //RegisterToEditorObjectContainer(skillPreviewCubeMeshObject);

        //auto skillPreviewSphereMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        //AttachDebugMesh(skillPreviewSphereMeshObject, DebugMeshType::Sphere)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
        //SkillPreviewSystem::Instance().SetRangePreviewObject(skillPreviewSphereMeshObject);
        //RegisterToEditorObjectContainer(skillPreviewSphereMeshObject);

        //InputManager::Instance();
        //UIManager::Instance();
        //bool mapFound = false;

        //editor::MapFileManager::GetSingletonInstance().LoadMapFile("InWanderLand.pmap");
        //editor::InstanceManager::GetSingletonInstance().ApplyInstancesAsPlaytimeObjects();

    }
#endif
#endif
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

void application::contents::ContentsLayer::PlayContents()
{
    SingletonInstanceContainer::SingleInstance().PermitCreateInstances();
    editor::InstanceManager::GetSingletonInstance().ApplyInstancesAsPlaytimeObjects();
    //UIManager::Instance().ImportUI("InWanderLand.iwui");
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

    //UIManager::Instance().LoadUITextures();
    UIManager::Instance().ImportUI("InWanderLand.iwui");

    auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

    const int inGameLayer = 1;

    auto menuPanel = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<UIPanel>();
    //SoundSystem::PlayMusic("Texture/UI/InGameUITemp/Game_BGM.mp3");

#pragma region MenuButton
    auto menuUIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto menuUIImageComponent = menuUIObject->AddComponent<UIImage>();
    auto menuUIButtonComponent = menuUIObject->AddComponent<UIButton>();
    menuUIButtonComponent->SetImageComponent(menuUIImageComponent);
    menuUIButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0000_Icon_Menu.png"));
    menuUIButtonComponent->SetLayer(100);
    menuUIButtonComponent->SetButtonClickFunction([=]()
        {
            if (!menuPanel->GetPanelActive())
                menuPanel->SetPanelActive(true);
            else
                menuPanel->SetPanelActive(false);
        });
    menuUIObject->GetTransform()->SetLocalPosition({ 0, 1000, 0 });
#pragma endregion

    const int penalLayer = 10;
    //const int buttonLayer = 11;

#pragma region Menu Window
    auto menuWindowUIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto menuWindowImageComponent = menuWindowUIObject->AddComponent<UIImage>();
    menuWindowImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame_Menu/PopUp_IngameMenu.png"));
    menuWindowImageComponent->GetGI().SetLayer(penalLayer);
    menuWindowUIObject->SetSelfActive(false);
    menuWindowUIObject->GetTransform()->SetWorldPosition({ menuWindowXpos, menuWindowYpos, 0 });
    menuPanel->SetWindowImage(menuWindowImageComponent);
#pragma endregion

#pragma region Game Manual Panel
    auto gameManualPanel = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<UIPanel>();
    auto gameManualWindowObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto gameManualWindowImageComponent = gameManualWindowObject->AddComponent<UIImage>();
    gameManualWindowImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0043_RollOver_RobinSkill1_0.png"));
    gameManualWindowImageComponent->GetGI().SetLayer(penalLayer);
    gameManualWindowObject->SetSelfActive(false);
    gameManualWindowObject->GetTransform()->SetWorldPosition({ menuWindowXpos, menuWindowYpos, 0 });
    gameManualPanel->SetWindowImage(gameManualWindowImageComponent);
#pragma endregion

#pragma region Game Manual Button
    {
        auto gameManualButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto gameManualImageComponent = gameManualButtonObject->AddComponent<UIImage>();
        auto gameManualButtonComponent = gameManualButtonObject->AddComponent<UIButton>();
        gameManualButtonComponent->SetImageComponent(gameManualImageComponent);
        gameManualButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/Ingame_Menu/Button_HowToPlay.png"));
        //gameManualButtonComponent->SetLayer(buttonLayer);
        gameManualButtonComponent->SetButtonClickFunction([=]()
            {
                gameManualPanel->SetPanelActive(true);
                menuPanel->SetPanelActive(false);
            });
        gameManualButtonObject->GetTransform()->SetLocalPosition({ menuWindowXpos + 200 - 100, menuWindowYpos + 83 - 25, 0 });
        gameManualButtonObject->SetSelfActive(false);
        menuPanel->SetUIButtonComponent(gameManualButtonComponent);
    }
#pragma endregion

#pragma region Sound Panel
    auto soundPanel = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<UIPanel>();
    auto soundWindowObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto soundWindowImageComponent = soundWindowObject->AddComponent<UIImage>();
    soundWindowImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0053_Window_AudioSetting.png"));
    soundWindowImageComponent->GetGI().SetLayer(penalLayer);
    soundWindowObject->SetSelfActive(false);
    soundWindowObject->GetTransform()->SetWorldPosition({ menuWindowXpos, menuWindowYpos, 0 });
    soundPanel->SetWindowImage(soundWindowImageComponent);
#pragma endregion

#pragma region Sound Button
    {
        auto soundButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto soundButtonImageComponent = soundButtonObject->AddComponent<UIImage>();
        auto soundButtonComponent = soundButtonObject->AddComponent<UIButton>();
        soundButtonComponent->SetImageComponent(soundButtonImageComponent);
        soundButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/Ingame_Menu/Button_Sound.png"));
        //soundButtonComponent->SetLayer(buttonLayer);
        soundButtonComponent->SetButtonClickFunction([=]()
            {
                soundPanel->SetPanelActive(true);
                menuPanel->SetPanelActive(false);
            });
        soundButtonObject->GetTransform()->SetLocalPosition({ menuWindowXpos + 200 - 100, menuWindowYpos + 158 - 25, 0 });
        soundButtonObject->SetSelfActive(false);
        menuPanel->SetUIButtonComponent(soundButtonComponent);
    }
#pragma endregion

#pragma region Restart Button
    {
        auto restartButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto restartButtonImageComponent = restartButtonObject->AddComponent<UIImage>();
        auto restartButtonComponent = restartButtonObject->AddComponent<UIButton>();
        restartButtonComponent->SetImageComponent(restartButtonImageComponent);
        restartButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/Ingame_Menu/Button_Restart.png"));
        //restartButtonComponent->SetLayer(penalLayer + 1);
        restartButtonComponent->SetButtonClickFunction([=]()
            {
                /// 게임을 재시작하는 로직
            });
        restartButtonObject->GetTransform()->SetLocalPosition({ menuWindowXpos + 200 - 100, menuWindowYpos + 233 - 25, 0 });
        restartButtonObject->SetSelfActive(false);
        menuPanel->SetUIButtonComponent(restartButtonComponent);
    }
#pragma endregion

#pragma region TacticMode UI
    auto tacticModeUIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto tacticModeImageComponent = tacticModeUIObject->AddComponent<UIImage>();
    tacticModeImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0001_Frame_Tactics.png"));
    tacticModeImageComponent->GetGI().SetLayer(inGameLayer);
    tacticModeUIObject->GetTransform()->SetWorldPosition({ 420 - 150 / 2, 1000 - 150 / 2, 0 });
#pragma endregion

#pragma region Robin State
    const int robinStateBarPosX = 656 - 300 / 2;
    const int robinStateBarPosY = 908 - 20 / 2;

    {
        auto robinStateBarObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto robinStateBarImageComponent = robinStateBarObject->AddComponent<UIImage>();
        robinStateBarImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0005_Frame_HpBar.png"));
        robinStateBarImageComponent->GetGI().SetLayer(inGameLayer);
        robinStateBarObject->GetTransform()->SetWorldPosition({ robinStateBarPosX, robinStateBarPosY, 0 });
    }

    {
        auto robinPortraitObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto robinPortraitImageComponent = robinPortraitObject->AddComponent<UIImage>();
        robinPortraitImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0020_Portrait_Robin.png"));
        robinPortraitImageComponent->GetGI().SetLayer(inGameLayer);
        robinPortraitObject->GetTransform()->SetWorldPosition({ robinStateBarPosX + 50 - 50, robinStateBarPosY + 73 - 50, 0 });
    }

    {
        auto robinSkill1UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto robinSkill1UIImageComponent = robinSkill1UIObject->AddComponent<UIImage>();
        robinSkill1UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0017_Skill_RobinQ.png"));
        robinSkill1UIImageComponent->GetGI().SetLayer(inGameLayer);
        robinSkill1UIObject->GetTransform()->SetWorldPosition({ robinStateBarPosX + 150 - 50, robinStateBarPosY + 73 - 50, 0 });
    }

    {
        auto robinSkill2UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto robinSkill2UIImageComponent = robinSkill2UIObject->AddComponent<UIImage>();
        robinSkill2UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0012_Skill_RobinE.png"));
        robinSkill2UIImageComponent->GetGI().SetLayer(inGameLayer);
        robinSkill2UIObject->GetTransform()->SetWorldPosition({ robinStateBarPosX + 250 - 50, robinStateBarPosY + 73 - 50, 0 });
    }
#pragma endregion

#pragma region Ursula State
    const int ursulaStateBarPosX = 1006 - 300 / 2;
    const int ursulaStateBarPosY = 908 - 20 / 2;

    {
        auto ursulaStateBarObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto ursulaStateBarImageComponent = ursulaStateBarObject->AddComponent<UIImage>();
        ursulaStateBarImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0005_Frame_HpBar.png"));
        ursulaStateBarImageComponent->GetGI().SetLayer(inGameLayer);
        ursulaStateBarObject->GetTransform()->SetWorldPosition({ ursulaStateBarPosX, ursulaStateBarPosY, 0 });
    }

    {
        auto ursulaPortraitObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto ursulaPortraitImageComponent = ursulaPortraitObject->AddComponent<UIImage>();
        ursulaPortraitImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0019_Portrait_Ursula.png"));
        ursulaPortraitImageComponent->GetGI().SetLayer(inGameLayer);
        ursulaPortraitObject->GetTransform()->SetWorldPosition({ ursulaStateBarPosX + 50 - 50, ursulaStateBarPosY + 73 - 50, 0 });
    }

    {
        auto ursulaSkill1UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto ursulaSkill1UIImageComponent = ursulaSkill1UIObject->AddComponent<UIImage>();
        ursulaSkill1UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0014_Skill_UrsulaQ.png"));
        ursulaSkill1UIImageComponent->GetGI().SetLayer(inGameLayer);
        ursulaSkill1UIObject->GetTransform()->SetWorldPosition({ ursulaStateBarPosX + 150 - 50, ursulaStateBarPosY + 73 - 50, 0 });
    }

    {
        auto ursulaSkill2UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto ursulaSkill2UIImageComponent = ursulaSkill2UIObject->AddComponent<UIImage>();
        ursulaSkill2UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0013_Skill_UrsulaE.png"));
        ursulaSkill2UIImageComponent->GetGI().SetLayer(inGameLayer);
        ursulaSkill2UIObject->GetTransform()->SetWorldPosition({ ursulaStateBarPosX + 250 - 50, ursulaStateBarPosY + 73 - 50, 0 });
    }
#pragma endregion

#pragma region Gretel State
    const int gretelStateBarPosX = 1357 - 300 / 2;
    const int gretelStateBarPosY = 908 - 20 / 2;

    {
        auto gretelStateBarObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto gretelStateBarImageComponent = gretelStateBarObject->AddComponent<UIImage>();
        gretelStateBarImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0005_Frame_HpBar.png"));
        gretelStateBarImageComponent->GetGI().SetLayer(inGameLayer);
        gretelStateBarObject->GetTransform()->SetWorldPosition({ gretelStateBarPosX, gretelStateBarPosY, 0 });
    }

    {
        auto gretelPortraitObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto gretelPortraitImageComponent = gretelPortraitObject->AddComponent<UIImage>();
        gretelPortraitImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0018_Portrait_Hansel.png"));
        gretelPortraitImageComponent->GetGI().SetLayer(inGameLayer);
        gretelPortraitObject->GetTransform()->SetWorldPosition({ gretelStateBarPosX + 50 - 50, gretelStateBarPosY + 73 - 50, 0 });
    }

    {
        auto gretelSkill1UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto gretelSkill1UIImageComponent = gretelSkill1UIObject->AddComponent<UIImage>();
        gretelSkill1UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0016_Skill_HanselQ.png"));
        gretelSkill1UIImageComponent->GetGI().SetLayer(inGameLayer);
        gretelSkill1UIObject->GetTransform()->SetWorldPosition({ gretelStateBarPosX + 150 - 50, gretelStateBarPosY + 73 - 50, 0 });
    }

    {
        auto gretelSkill2UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto gretelSkill2UIImageComponent = gretelSkill2UIObject->AddComponent<UIImage>();
        gretelSkill2UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0015_Skill_HanselE.png"));
        gretelSkill2UIImageComponent->GetGI().SetLayer(inGameLayer);
        gretelSkill2UIObject->GetTransform()->SetWorldPosition({ gretelStateBarPosX + 250 - 50, gretelStateBarPosY + 73 - 50, 0 });
    }
#pragma endregion

#pragma region Combo Related UI
    const int comboUIPosX = 1600;
    const int comboUIPosY = 500;
    {
        auto comboQuest1Object = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto comboQuest1UIImageComponent = comboQuest1Object->AddComponent<UIImage>();
        comboQuest1UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0029_Quest1_UnComplete.png"));
        comboQuest1UIImageComponent->GetGI().SetLayer(inGameLayer);
        comboQuest1Object->GetTransform()->SetWorldPosition({ comboUIPosX, comboUIPosY, 0 });
    }

    {
        auto comboQuest2Object = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto comboQuest2UIImageComponent = comboQuest2Object->AddComponent<UIImage>();
        comboQuest2UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0023_Quest2_UnCompleted.png"));
        comboQuest2UIImageComponent->GetGI().SetLayer(inGameLayer);
        comboQuest2Object->GetTransform()->SetWorldPosition({ comboUIPosX, comboUIPosY + 40, 0 });
    }

    {
        auto comboQuest3Object = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto comboQuest3UIImageComponent = comboQuest3Object->AddComponent<UIImage>();
        comboQuest3UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0028_Quest3_UnComplete.png"));
        comboQuest3UIImageComponent->GetGI().SetLayer(inGameLayer);
        comboQuest3Object->GetTransform()->SetWorldPosition({ comboUIPosX, comboUIPosY + 80, 0 });
    }

    {
        auto comboTextObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto comboTextUIImageComponent = comboTextObject->AddComponent<UIImage>();
        comboTextUIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0040_System_ComboRate.png"));
        comboTextUIImageComponent->GetGI().SetLayer(inGameLayer);
        comboTextObject->GetTransform()->SetWorldPosition({ comboUIPosX + 120, comboUIPosY - 40, 0 });
    }

#pragma endregion


    menuPanel->SetCloseButtonActive(true);
    gameManualPanel->SetCloseButtonActive(true);
    soundPanel->SetCloseButtonActive(true);

    gameManualPanel->SetParentPanel(menuPanel);
    soundPanel->SetParentPanel(menuPanel);

    auto robinPassiveActivateButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto robinPassiveActivateImage = robinPassiveActivateButtonObject->AddComponent<UIImage>();
    auto robinPassiveActivateButton = robinPassiveActivateButtonObject->AddComponent<UIButton>();
    robinPassiveActivateButton->SetImageComponent(robinPassiveActivateImage);
    robinPassiveActivateButton->SetIdleImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0001_Frame_Tactics.png"));
    robinPassiveActivateButton->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0001_Frame_Tactics.png"));
    robinPassiveActivateButton->SetClickedImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0001_Frame_Tactics.png"));
    robinPassiveActivateButton->SetLayer(10.0);
    RobinSkillDevelopmentSystem::Instance().AddTopLayerButton(robinPassiveActivateButton);
    robinPassiveActivateButtonObject->GetTransform()->SetLocalPosition({ 0, 700, 0 });
    RegisterToEditorObjectContainer(robinPassiveActivateButtonObject);

    auto robinWSkillUpgradeButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto robinWSkillUpgradeImage = robinWSkillUpgradeButtonObject->AddComponent<UIImage>();
    auto robinWSkillUpgradeButton = robinWSkillUpgradeButtonObject->AddComponent<UIButton>();
    robinWSkillUpgradeButton->SetImageComponent(robinWSkillUpgradeImage);
    robinWSkillUpgradeButton->SetIdleImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0001_Frame_Tactics.png"));
    robinWSkillUpgradeButton->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0001_Frame_Tactics.png"));
    robinWSkillUpgradeButton->SetClickedImage(rsrcMgr->GetTexture(L"Texture/Ingame/_0001_Frame_Tactics.png"));
    robinWSkillUpgradeButton->SetLayer(10.0);
    RobinSkillDevelopmentSystem::Instance().AddMiddleLayerButton(robinWSkillUpgradeButton);
    robinWSkillUpgradeButtonObject->GetTransform()->SetLocalPosition({ 300, 700, 0 });
    RegisterToEditorObjectContainer(robinWSkillUpgradeButtonObject);

    /// Playable 동작들을 일괄 처리할 부분입니다.
    PlayableComponent::OnGameStartAll();
}

void application::contents::ContentsLayer::PauseContents()
{
    Time::SetTimeScale(FLT_MIN * 1000);
    for (auto e : componentsCreatedByEditorVector)
    {
        e->SetActive(false);
    }

    /// Playable 동작들을 일괄 처리할 부분입니다.
    PlayableComponent::OnGamePauseAll();
}

void application::contents::ContentsLayer::ResumeContents()
{
    Time::SetTimeScale(1);
    for (auto e : componentsCreatedByEditorVector)
    {
        e->SetActive(true);
    }

    /// Playable 동작들을 일괄 처리할 부분입니다.
    PlayableComponent::OnGameResumeAll();
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

    UIManager::Instance().Clear();

    /// Playable 동작들을 일괄 처리할 부분입니다.
    PlayableComponent::OnGameStopAll();
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
    /// 카메라가 추적하고 있는 
    /// 생성된 모든 게임 플레이 오브젝트 삭제
    for (auto e : objectCreatedByEditorList)
    {
        //      if (e->getName() == "")
              //e->SetSelfActive(false);
        yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(e);
    }

    for (auto e : componentsCreatedByEditorVector)
    {
        e->SetActive(false);
    }
    objectCreatedByEditorList.clear();

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

void application::contents::ContentsLayer::RegisterToEditorObjectContainer(GameObject* p_obj)
{
    objectCreatedByEditorList.push_back(p_obj);
}

void application::contents::ContentsLayer::RegisterToEditorComponentVector(Component* p_com)
{
    componentsCreatedByEditorVector.push_back(p_com);
}
