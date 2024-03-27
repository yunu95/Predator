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
#include "InputManager.h"
#include "UIManager.h"
#include "PlayerController.h"
#include "TacticModeSystem.h"
#include "SingletonInstanceContainer.h"
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
    }
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

    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Robin");
    //	obj->GetTransform()->SetLocalPosition({ Vector3d{0,0,-5} });
    //}

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

    //auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //camObj->GetTransform()->SetLocalPosition(Vector3d(0, 20, -10));
    //camObj->GetTransform()->SetLocalRotation( Quaternion(Vector3d(60, 0, 0)));
    //auto roamingCam = camObj->AddComponent<RTSCam>();
    //roamingCam->SetCameraMain();

    wanderUtils::LoadResourcesRecursively();
    //const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

    //resourceManager->LoadFile("FBXMaterial.scres");

    //resourceManager->LoadFile("LeavesVS.cso");
    //resourceManager->LoadFile("LeavesPS.cso");
    //resourceManager->LoadFile("Stage_1_FloorPS.cso");

    //resourceManager->LoadFile("Texture/VertexColor/T_Dirt_ARM.png");
    //resourceManager->LoadFile("Texture/VertexColor/T_Dirt_BaseColor.png");
    //resourceManager->LoadFile("Texture/VertexColor/T_Dirt_Normal.png");

    //resourceManager->LoadFile("Texture/VertexColor/T_Grass_ARM.png");
    //resourceManager->LoadFile("Texture/VertexColor/T_Grass_BaseColor.png");
    //resourceManager->LoadFile("Texture/VertexColor/T_Grass_Normal.png");

    //resourceManager->LoadFile("Texture/VertexColor/T_GrassBlend_ARM.png");
    //resourceManager->LoadFile("Texture/VertexColor/T_GrassBlend_BaseColor.png");
    //resourceManager->LoadFile("Texture/VertexColor/T_GrassBlend_Normal.png");

    //resourceManager->LoadFile("Texture/VertexColor/T_Tile_ARM.png");
    //resourceManager->LoadFile("Texture/VertexColor/T_Tile_BaseColor.png");
    //resourceManager->LoadFile("Texture/VertexColor/T_Tile_Normal.png");

    //resourceManager->LoadFile("Texture/VertexColor/T_TileBlend_ARM.png");
    //resourceManager->LoadFile("Texture/VertexColor/T_TileBlend_BaseColor.png");
    //resourceManager->LoadFile("Texture/VertexColor/T_TileBlend_Normal.png");

    //resourceManager->LoadFile("FBX/SM_VertexColor");
    //resourceManager->LoadFile("FBX/SKM_Monster1");
    //resourceManager->LoadFile("FBX/SKM_Monster2");
    //resourceManager->LoadFile("FBX/SKM_Robin");
    //resourceManager->LoadFile("FBX/SM_Bush_001");
    //resourceManager->LoadFile("FBX/SM_Bush_002");
    //resourceManager->LoadFile("FBX/SM_Trunk_001");
    //resourceManager->LoadFile("FBX/SM_CastleWall");
    //resourceManager->LoadFile("FBX/SM_CastleWall_Door");
    //resourceManager->LoadFile("FBX/SM_CastleWall_Pillar");
    //resourceManager->LoadFile("FBX/SM_Chair");
    //resourceManager->LoadFile("FBX/SM_CupTower");
    //resourceManager->LoadFile("FBX/SM_Fork");
    //resourceManager->LoadFile("FBX/SM_GuideBook");
    //resourceManager->LoadFile("FBX/SM_Hat01");
    //resourceManager->LoadFile("FBX/SM_Hat02");
    //resourceManager->LoadFile("FBX/SM_SmallBush_001");
    //resourceManager->LoadFile("FBX/SM_Stone_001");
    //resourceManager->LoadFile("FBX/SM_Stone_002");
    //resourceManager->LoadFile("FBX/SM_Stump");
    //resourceManager->LoadFile("FBX/SM_Temple_Book_etc");
    //resourceManager->LoadFile("FBX/SM_Temple_Books");
    //resourceManager->LoadFile("FBX/SM_Temple_Floor");
    //resourceManager->LoadFile("FBX/SM_Temple_Pillar");
    //resourceManager->LoadFile("FBX/SM_Temple_Pillar_Broken");
    //resourceManager->LoadFile("FBX/SM_Temple_Rabbit");
    //resourceManager->LoadFile("FBX/SM_Mushroom01");
    //resourceManager->LoadFile("FBX/SM_Mushroom02");
    //resourceManager->LoadFile("FBX/SM_Temple_Welcome");
    //resourceManager->LoadFile("FBX/SM_Stage1_Floor");

#ifndef EDITOR
#ifdef GRAPHICS_TEST
    {
        yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
        //yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
        yunutyEngine::Collider2D::SetIsOnXYPlane(false);
        auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        directionalLight->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{50,-30,0} });
        directionalLight->GetTransform()->SetLocalPosition(Vector3d{ 0,0,-20 });
        auto light = directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
        auto color = yunuGI::Color{ 1,1,1,1.f };
        light->GetGI().SetLightDiffuseColor(color);

        editor::MapFileManager::GetSingletonInstance().LoadStaticOrnaments("TestOrnaments.punreal");
    }
    GraphicsTest();
#else
    {
        yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

		/// Editor 에서 수정하여 Map Data 에 저장할 부분
		/*auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto camComp = camObj->AddComponent<graphics::Camera>();
		camComp->SetCameraMain();*/
		//camObj->GetTransform()->SetLocalPosition({ 0,25,0 });
		//camObj->AddComponent<Dotween>();
		//RegisterToEditorObjectVector(camObj);

        //auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        //directionalLight->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{50,-30,0} });
        //directionalLight->GetTransform()->SetLocalPosition(Vector3d{ 0,0,-20 });
        //auto light = directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();

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

        //camComp->groundHoveringClickCallback = [=](Vector3d pos)
        //    {
        //        mouseCursorObject->GetTransform()->SetWorldPosition(pos);
        //        SkillPreviewSystem::Instance().SetCurrentMousPosition(pos);
        //    };

        InputManager::Instance();
        UIManager::Instance();
        //PlayerController::SingleInstance().SetMovingSystemComponent(camComp);
        //TacticModeSystem::SingleInstance().SetMovingSystemComponent(camComp);

        // UIButton Test
        //rsrcMgr->LoadFile("Texture/zoro.jpg");
        //rsrcMgr->LoadFile("Texture/zoro_highLighted.jpg");
        //rsrcMgr->LoadFile("Texture/zoro_Clicked.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Menu.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/TacticMode.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Robin_ParentUI.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Ursula_ParentUI.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Gretel_ParentUI.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Robin_Portrait.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Ursula_Portrait.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Gretel_Portrait.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Robin_Skill1_Ikon.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Ursula_Skill1_Ikon.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Gretel_Skill1_Ikon.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Robin_Skill2_Ikon.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Ursula_Skill2_Ikon.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Gretel_Skill2_Ikon.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/CloseButton.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/menu_window.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/GameManual_Button.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/GameManual.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Sound_Window.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Sound_Button.jpg");
        //rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Restart_Button.jpg");
        //yunutyEngine::NavigationAgent* agent = nullptr;
        //{
        //	agent = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationAgent>();
        //	agent->GetTransform()->SetLocalPosition(Vector3d{ 0,0,20 });
        //	agent->SetSpeed(5);
        //	agent->SetRadius(0.5);
        //	agent->AssignToNavigationField(&SingleNavigationField::Instance());
        //	auto staticMesh = agent->GetGameObject()->AddGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        //	staticMesh->GetGI().SetMesh(graphics::Renderer::SingleInstance().GetResourceManager()->GetMesh(L"Capsule"));
        //	staticMesh->GetGI().GetMaterial()->SetColor({ 0.75,0.75,0.75,1 });
        //	staticMesh->GetTransform()->SetLocalPosition(Vector3d{ 0,0.5,0 });
        //}
        bool mapFound = false;
        /*for (const auto& entry : std::filesystem::directory_iterator("./"))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".pmap")
            {
                editor::MapFileManager::GetSingletonInstance().LoadMapFile(entry.path().filename().string());
                mapFound = true;
                break;
            }
        }*/
        //assert(mapFound && "there is no map to load in current directory!");

#pragma region UI Region

		    /// UIButton Test
		    rsrcMgr->LoadFile("Texture/zoro.jpg");
		  //  rsrcMgr->LoadFile("Texture/zoro_highLighted.jpg");
		  //  rsrcMgr->LoadFile("Texture/zoro_Clicked.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Menu.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/TacticMode.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Robin_ParentUI.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Ursula_ParentUI.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Gretel_ParentUI.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Robin_Portrait.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Ursula_Portrait.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Gretel_Portrait.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Robin_Skill1_Ikon.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Ursula_Skill1_Ikon.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Gretel_Skill1_Ikon.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Robin_Skill2_Ikon.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Ursula_Skill2_Ikon.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Gretel_Skill2_Ikon.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/CloseButton.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/menu_window.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/GameManual_Button.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/GameManual.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Sound_Window.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Sound_Button.jpg");
		  //  rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Restart_Button.jpg");


		  //  const int inGameLayer = 1;

		  //  auto menuPanel = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<UIPanel>();
		  //  //SoundSystem::PlayMusic("Texture/UI/InGameUITemp/Game_BGM.mp3");

    //#pragma region MenuButton
		  //  auto menuUIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		  //  auto menuUIImageComponent = menuUIObject->AddComponent<yunutyEngine::graphics::UIImage>();
		  //  auto menuUIButtonComponent = menuUIObject->AddComponent<UIButton>();
		  //  menuUIButtonComponent->SetImageComponent(menuUIImageComponent);
		  //  menuUIButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Menu.jpg"));
		  //  menuUIButtonComponent->SetLayer(100);
		  //  menuUIButtonComponent->SetButtonClickFunction([=]()
			 //   {
				//    menuPanel->SetPanelActive(true);
				//    SoundSystem::PlaySoundfile("Texture/UI/InGameUITemp/TestClick2.wav");
				//    SoundSystem::PlaySoundfile("Texture/UI/InGameUITemp/TestClick.wav");
			 //   });
		  //  menuUIObject->GetTransform()->SetLocalPosition({ 0, 1000, 0 });
    //#pragma endregion

		  //  const int penalLayer = 10;
		  //  //const int buttonLayer = 11;

    //#pragma region Menu Window
		  //  const int menuWindowXpos = 760;
		  //  const int menuWindowYpos = 540 - 350 / 2;

		  //  auto menuWindowUIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		  //  auto menuWindowImageComponent = menuWindowUIObject->AddComponent<yunutyEngine::graphics::UIImage>();
		  //  menuWindowImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/menu_window.jpg"));
		  //  menuWindowImageComponent->GetGI().SetLayer(penalLayer);
		  //  menuWindowUIObject->SetSelfActive(false);
		  //  menuWindowUIObject->GetTransform()->SetWorldPosition({ menuWindowXpos, menuWindowYpos, 0 });
		  //  menuPanel->SetWindowImage(menuWindowImageComponent);
    //#pragma endregion

    //#pragma region Game Manual Panel
		  //  auto gameManualPanel = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<UIPanel>();
		  //  auto gameManualWindowObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		  //  auto gameManualWindowImageComponent = gameManualWindowObject->AddComponent<yunutyEngine::graphics::UIImage>();
		  //  gameManualWindowImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/GameManual.jpg"));
		  //  gameManualWindowImageComponent->GetGI().SetLayer(penalLayer);
		  //  gameManualWindowObject->SetSelfActive(false);
		  //  gameManualWindowObject->GetTransform()->SetWorldPosition({ menuWindowXpos, menuWindowYpos, 0 });
		  //  gameManualPanel->SetWindowImage(gameManualWindowImageComponent);
    //#pragma endregion

    //#pragma region Game Manual Button
		  //  {
			 //   auto gameManualButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto gameManualImageComponent = gameManualButtonObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   auto gameManualButtonComponent = gameManualButtonObject->AddComponent<UIButton>();
			 //   gameManualButtonComponent->SetImageComponent(gameManualImageComponent);
			 //   gameManualButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/GameManual_Button.jpg"));
			 //   //gameManualButtonComponent->SetLayer(buttonLayer);
			 //   gameManualButtonComponent->SetButtonClickFunction([=]()
				//    {
				//	    gameManualPanel->SetPanelActive(true);
				//	    menuPanel->SetPanelActive(false);
				//    });
			 //   gameManualButtonObject->GetTransform()->SetLocalPosition({ menuWindowXpos + 200 - 100, menuWindowYpos + 83 - 25, 0 });
			 //   gameManualButtonObject->SetSelfActive(false);
			 //   menuPanel->SetUIButtonComponent(gameManualButtonComponent);
		  //  }
    //#pragma endregion

    //#pragma region Sound Panel
		  //  auto soundPanel = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<UIPanel>();
		  //  auto soundWindowObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		  //  auto soundWindowImageComponent = soundWindowObject->AddComponent<yunutyEngine::graphics::UIImage>();
		  //  soundWindowImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Sound_Window.jpg"));
		  //  soundWindowImageComponent->GetGI().SetLayer(penalLayer);
		  //  soundWindowObject->SetSelfActive(false);
		  //  soundWindowObject->GetTransform()->SetWorldPosition({ menuWindowXpos, menuWindowYpos, 0 });
		  //  soundPanel->SetWindowImage(soundWindowImageComponent);
    //#pragma endregion

    //#pragma region Sound Button
		  //  {
			 //   auto soundButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto soundButtonImageComponent = soundButtonObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   auto soundButtonComponent = soundButtonObject->AddComponent<UIButton>();
			 //   soundButtonComponent->SetImageComponent(soundButtonImageComponent);
			 //   soundButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Sound_Button.jpg"));
			 //   //soundButtonComponent->SetLayer(buttonLayer);
			 //   soundButtonComponent->SetButtonClickFunction([=]()
				//    {
				//	    soundPanel->SetPanelActive(true);
				//	    menuPanel->SetPanelActive(false);
				//    });
			 //   soundButtonObject->GetTransform()->SetLocalPosition({ menuWindowXpos + 200 - 100, menuWindowYpos + 158 - 25, 0 });
			 //   soundButtonObject->SetSelfActive(false);
			 //   menuPanel->SetUIButtonComponent(soundButtonComponent);
		  //  }
    //#pragma endregion

    //#pragma region Restart Button
		  //  {
			 //   auto restartButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto restartButtonImageComponent = restartButtonObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   auto restartButtonComponent = restartButtonObject->AddComponent<UIButton>();
			 //   restartButtonComponent->SetImageComponent(restartButtonImageComponent);
			 //   restartButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Restart_Button.jpg"));
			 //   //restartButtonComponent->SetLayer(penalLayer + 1);
			 //   restartButtonComponent->SetButtonClickFunction([=]()
				//    {
				//	    /// 게임을 재시작하는 로직
				//    });
			 //   restartButtonObject->GetTransform()->SetLocalPosition({ menuWindowXpos + 200 - 100, menuWindowYpos + 233 - 25, 0 });
			 //   restartButtonObject->SetSelfActive(false);
			 //   menuPanel->SetUIButtonComponent(restartButtonComponent);
		  //  }
    //#pragma endregion

    //#pragma region TacticMode UI
		  //  auto tacticModeUIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		  //  auto tacticModeImageComponent = tacticModeUIObject->AddComponent<yunutyEngine::graphics::UIImage>();
		  //  tacticModeImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/TacticMode.jpg"));
		  //  tacticModeImageComponent->GetGI().SetLayer(inGameLayer);
		  //  tacticModeUIObject->GetTransform()->SetWorldPosition({ 420 - 150 / 2, 1000 - 150 / 2, 0 });
    //#pragma endregion

    //#pragma region Robin State
		  //  const int robinStateBarPosX = 656 - 300 / 2;
		  //  const int robinStateBarPosY = 908 - 20 / 2;

		  //  {
			 //   auto robinStateBarObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto robinStateBarImageComponent = robinStateBarObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   robinStateBarImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Robin_ParentUI.jpg"));
			 //   robinStateBarImageComponent->GetGI().SetLayer(inGameLayer);
			 //   robinStateBarObject->GetTransform()->SetWorldPosition({ robinStateBarPosX, robinStateBarPosY, 0 });
		  //  }

		  //  {
			 //   auto robinPortraitObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto robinPortraitImageComponent = robinPortraitObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   robinPortraitImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Robin_Portrait.jpg"));
			 //   robinPortraitImageComponent->GetGI().SetLayer(inGameLayer);
			 //   robinPortraitObject->GetTransform()->SetWorldPosition({ robinStateBarPosX + 50 - 50, robinStateBarPosY + 73 - 50, 0 });
		  //  }

		  //  {
			 //   auto robinSkill1UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto robinSkill1UIImageComponent = robinSkill1UIObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   robinSkill1UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Robin_Skill1_Ikon.jpg"));
			 //   robinSkill1UIImageComponent->GetGI().SetLayer(inGameLayer);
			 //   robinSkill1UIObject->GetTransform()->SetWorldPosition({ robinStateBarPosX + 150 - 50, robinStateBarPosY + 73 - 50, 0 });
		  //  }

		  //  {
			 //   auto robinSkill2UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto robinSkill2UIImageComponent = robinSkill2UIObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   robinSkill2UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Robin_Skill2_Ikon.jpg"));
			 //   robinSkill2UIImageComponent->GetGI().SetLayer(inGameLayer);
			 //   robinSkill2UIObject->GetTransform()->SetWorldPosition({ robinStateBarPosX + 250 - 50, robinStateBarPosY + 73 - 50, 0 });
		  //  }
    //#pragma endregion

    //#pragma region Ursula State
		  //  const int ursulaStateBarPosX = 1006 - 300 / 2;
		  //  const int ursulaStateBarPosY = 908 - 20 / 2;

		  //  {
			 //   auto ursulaStateBarObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto ursulaStateBarImageComponent = ursulaStateBarObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   ursulaStateBarImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Ursula_ParentUI.jpg"));
			 //   ursulaStateBarImageComponent->GetGI().SetLayer(inGameLayer);
			 //   ursulaStateBarObject->GetTransform()->SetWorldPosition({ ursulaStateBarPosX, ursulaStateBarPosY, 0 });
		  //  }

		  //  {
			 //   auto ursulaPortraitObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto ursulaPortraitImageComponent = ursulaPortraitObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   ursulaPortraitImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Ursula_Portrait.jpg"));
			 //   ursulaPortraitImageComponent->GetGI().SetLayer(inGameLayer);
			 //   ursulaPortraitObject->GetTransform()->SetWorldPosition({ ursulaStateBarPosX + 50 - 50, ursulaStateBarPosY + 73 - 50, 0 });
		  //  }

		  //  {
			 //   auto ursulaSkill1UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto ursulaSkill1UIImageComponent = ursulaSkill1UIObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   ursulaSkill1UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Ursula_Skill1_Ikon.jpg"));
			 //   ursulaSkill1UIImageComponent->GetGI().SetLayer(inGameLayer);
			 //   ursulaSkill1UIObject->GetTransform()->SetWorldPosition({ ursulaStateBarPosX + 150 - 50, ursulaStateBarPosY + 73 - 50, 0 });
		  //  }

		  //  {
			 //   auto ursulaSkill2UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto ursulaSkill2UIImageComponent = ursulaSkill2UIObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   ursulaSkill2UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Ursula_Skill2_Ikon.jpg"));
			 //   ursulaSkill2UIImageComponent->GetGI().SetLayer(inGameLayer);
			 //   ursulaSkill2UIObject->GetTransform()->SetWorldPosition({ ursulaStateBarPosX + 250 - 50, ursulaStateBarPosY + 73 - 50, 0 });
		  //  }
    //#pragma endregion

    //#pragma region Gretel State
		  //  const int gretelStateBarPosX = 1357 - 300 / 2;
		  //  const int gretelStateBarPosY = 908 - 20 / 2;

		  //  {
			 //   auto gretelStateBarObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto gretelStateBarImageComponent = gretelStateBarObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   gretelStateBarImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Gretel_ParentUI.jpg"));
			 //   gretelStateBarImageComponent->GetGI().SetLayer(inGameLayer);
			 //   gretelStateBarObject->GetTransform()->SetWorldPosition({ gretelStateBarPosX, gretelStateBarPosY, 0 });
		  //  }

		  //  {
			 //   auto gretelPortraitObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto gretelPortraitImageComponent = gretelPortraitObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   gretelPortraitImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Gretel_Portrait.jpg"));
			 //   gretelPortraitImageComponent->GetGI().SetLayer(inGameLayer);
			 //   gretelPortraitObject->GetTransform()->SetWorldPosition({ gretelStateBarPosX + 50 - 50, gretelStateBarPosY + 73 - 50, 0 });
		  //  }

		  //  {
			 //   auto gretelSkill1UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto gretelSkill1UIImageComponent = gretelSkill1UIObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   gretelSkill1UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Gretel_Skill1_Ikon.jpg"));
			 //   gretelSkill1UIImageComponent->GetGI().SetLayer(inGameLayer);
			 //   gretelSkill1UIObject->GetTransform()->SetWorldPosition({ gretelStateBarPosX + 150 - 50, gretelStateBarPosY + 73 - 50, 0 });
		  //  }

		  //  {
			 //   auto gretelSkill2UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			 //   auto gretelSkill2UIImageComponent = gretelSkill2UIObject->AddComponent<yunutyEngine::graphics::UIImage>();
			 //   gretelSkill2UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Gretel_Skill2_Ikon.jpg"));
			 //   gretelSkill2UIImageComponent->GetGI().SetLayer(inGameLayer);
			 //   gretelSkill2UIObject->GetTransform()->SetWorldPosition({ gretelStateBarPosX + 250 - 50, gretelStateBarPosY + 73 - 50, 0 });
		  //  }
    //#pragma endregion

		  //  menuPanel->SetCloseButtonActive(true);
		  //  gameManualPanel->SetCloseButtonActive(true);
		  //  soundPanel->SetCloseButtonActive(true);

		  //  gameManualPanel->SetParentPanel(menuPanel);
		  //  soundPanel->SetParentPanel(menuPanel);

		    auto robinPassiveActivateButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		    auto robinPassiveActivateImage = robinPassiveActivateButtonObject->AddComponent<yunutyEngine::graphics::UIImage>();
		    auto robinPassiveActivateButton = robinPassiveActivateButtonObject->AddComponent<UIButton>();
		    robinPassiveActivateButton->SetImageComponent(robinPassiveActivateImage);
		    robinPassiveActivateButton->SetIdleImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
		    robinPassiveActivateButton->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
		    robinPassiveActivateButton->SetClickedImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
		    robinPassiveActivateButton->SetLayer(10.0);

		    RobinSkillDevelopmentSystem::Instance().AddTopLayerButton(robinPassiveActivateButton);
		    robinPassiveActivateButtonObject->GetTransform()->SetLocalPosition({ 0, 700, 0 });


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
#pragma endregion


        editor::MapFileManager::GetSingletonInstance().LoadMapFile("InWanderLand.pmap");
        editor::InstanceManager::GetSingletonInstance().ApplyInstancesAsPlaytimeObjects();

    }
#endif
#endif // ! EDITOR
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

	InputManager::Instance();
	UIManager::Instance();

    /// Editor 에서 수정하여 Map Data 에 저장할 부분

    /// 카메라의 경우 CameraData 의 ApplyInstancesAsPlaytimeObjects 에서 처리함

    auto skillPreviewCubeMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    AttachDebugMesh(skillPreviewCubeMeshObject, DebugMeshType::Cube)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
    SkillPreviewSystem::Instance().SetPathPreviewObject(skillPreviewCubeMeshObject);
    RegisterToEditorObjectVector(skillPreviewCubeMeshObject);

    auto skillPreviewSphereMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    AttachDebugMesh(skillPreviewSphereMeshObject, DebugMeshType::Sphere)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
    SkillPreviewSystem::Instance().SetRangePreviewObject(skillPreviewSphereMeshObject);
    RegisterToEditorObjectVector(skillPreviewSphereMeshObject);

    /// UI 작업
	const int menuWindowXpos = 760;
	const int menuWindowYpos = 540 - 350 / 2;

	auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	rsrcMgr->LoadFile("Texture/zoro.jpg");
	rsrcMgr->LoadFile("Texture/zoro_highLighted.jpg");
	rsrcMgr->LoadFile("Texture/zoro_Clicked.jpg");

    auto robinPassiveActivateButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto robinPassiveActivateImage = robinPassiveActivateButtonObject->AddComponent<yunutyEngine::graphics::UIImage>();
    auto robinPassiveActivateButton = robinPassiveActivateButtonObject->AddComponent<UIButton>();
    robinPassiveActivateButton->SetImageComponent(robinPassiveActivateImage);
    robinPassiveActivateButton->SetIdleImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
    robinPassiveActivateButton->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/zoro_highLighted.jpg"));
    robinPassiveActivateButton->SetClickedImage(rsrcMgr->GetTexture(L"Texture/zoro_Clicked.jpg"));

    RobinSkillDevelopmentSystem::Instance().AddTopLayerButton(robinPassiveActivateButton);
    robinPassiveActivateButtonObject->GetTransform()->SetLocalPosition({ menuWindowXpos + 200 - 100, menuWindowYpos + 83 - 25, 0 });

}

void application::contents::ContentsLayer::PauseContents()
{
    Time::SetTimeScale(FLT_MIN * 1000);
}

void application::contents::ContentsLayer::ResumeContents()
{
    Time::SetTimeScale(1);
}

void application::contents::ContentsLayer::StopContents()
{
    Time::SetTimeScale(1);
    isStoppedOnce = true;
    ClearPlaytimeObject();
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
	objectCreatedByEditorVector.clear();

    SingletonInstanceContainer::SingleInstance().ClearLazySingletonInstances();
}

void application::contents::ContentsLayer::RegisterToEditorObjectVector(GameObject* p_obj)
{
    objectCreatedByEditorVector.push_back(p_obj);
}

