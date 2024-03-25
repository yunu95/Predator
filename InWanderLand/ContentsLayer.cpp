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

#include <algorithm>
#include <string>
#include <filesystem>
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

    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //	obj->GetTransform()->SetLocalPosition(Vector3d{ -47.56,3.67,44.81 });
    //	auto light = obj->AddComponent<yunutyEngine::graphics::PointLight>();
    //	yunuGI::Color color{ 0,0,1.f,1 };
    //	light->GetGI().SetLightDiffuseColor(color);
    //	light->GetGI().SetRange(10);
    //}

    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //	obj->GetTransform()->SetLocalPosition(Vector3d{ -47.56,3.67,45.32 });
    //	auto light = obj->AddComponent<yunutyEngine::graphics::PointLight>();
    //	yunuGI::Color color{ 1.f,0,0,1 };
    //	light->GetGI().SetLightDiffuseColor(color);
    //	light->GetGI().SetRange(10);
    //}

    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //	obj->GetTransform()->SetLocalScale(Vector3d{ 100,1,100 });
    //	auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    //	renderer->GetGI().SetMesh(mesh);
    //}
    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //	obj->GetTransform()->SetLocalScale(Vector3d{ 100,100,1 });
    //	obj->GetTransform()->SetLocalPosition(Vector3d{ 0,50,50 });
    //	auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    //	renderer->GetGI().SetMesh(mesh);
    //}
    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //	obj->GetTransform()->SetLocalScale(Vector3d{ 1,100,100 });
    //	obj->GetTransform()->SetLocalPosition(Vector3d{ -50,50,0 });
    //	auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    //	renderer->GetGI().SetMesh(mesh);
    //}

    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //	obj->GetTransform()->SetLocalPosition(Vector3d{ -48.67,3.67,44.81 });
    //	auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    //	renderer->GetGI().SetMesh(mesh);
    //}
    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //	obj->GetTransform()->SetLocalPosition(Vector3d{ -47.26,3.67,47.83 });
    //	auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    //	renderer->GetGI().SetMesh(mesh);
    //}
    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //	obj->GetTransform()->SetLocalPosition(Vector3d{ -48.08,5.32,46.24 });
    //	auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    //	renderer->GetGI().SetMesh(mesh);
    //}

    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //	obj->GetTransform()->SetLocalScale(Vector3d{ 1,100,100 });
    //	obj->GetTransform()->SetLocalPosition(Vector3d{ 48.08,5.32,46.24 });
    //	auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    //	renderer->GetGI().SetMesh(mesh);
    //}

    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Monster2");
    //	auto animator = obj->GetComponent<yunutyEngine::graphics::Animator>();
    //	animator->GetGI().PushAnimation(animation);
    //	animator->GetGI().Play(animation);
    //	obj->GetTransform()->SetLocalPosition(Vector3d{ -47.55, 0.5f,42.53 });
    //}


    //{
    //	auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //	auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    //	renderer->GetGI().SetMesh(mesh);
    //}
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

    {
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

    //auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //camObj->GetTransform()->SetLocalPosition(Vector3d(0, 20, -10));
    //camObj->GetTransform()->SetLocalRotation( Quaternion(Vector3d(60, 0, 0)));
    //auto roamingCam = camObj->AddComponent<RTSCam>();
    //roamingCam->SetCameraMain();

    const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	resourceManager->LoadFile("FBXMaterial.scres");

    resourceManager->LoadFile("LeavesVS.cso");
    resourceManager->LoadFile("LeavesPS.cso");
    resourceManager->LoadFile("Stage_1_FloorPS.cso");

    resourceManager->LoadFile("Texture/VertexColor/T_Dirt_ARM.png");
    resourceManager->LoadFile("Texture/VertexColor/T_Dirt_BaseColor.png");
    resourceManager->LoadFile("Texture/VertexColor/T_Dirt_Normal.png");

    resourceManager->LoadFile("Texture/VertexColor/T_Grass_ARM.png");
    resourceManager->LoadFile("Texture/VertexColor/T_Grass_BaseColor.png");
    resourceManager->LoadFile("Texture/VertexColor/T_Grass_Normal.png");

    resourceManager->LoadFile("Texture/VertexColor/T_GrassBlend_ARM.png");
    resourceManager->LoadFile("Texture/VertexColor/T_GrassBlend_BaseColor.png");
    resourceManager->LoadFile("Texture/VertexColor/T_GrassBlend_Normal.png");

    resourceManager->LoadFile("Texture/VertexColor/T_Tile_ARM.png");
    resourceManager->LoadFile("Texture/VertexColor/T_Tile_BaseColor.png");
    resourceManager->LoadFile("Texture/VertexColor/T_Tile_Normal.png");

    resourceManager->LoadFile("Texture/VertexColor/T_TileBlend_ARM.png");
    resourceManager->LoadFile("Texture/VertexColor/T_TileBlend_BaseColor.png");
    resourceManager->LoadFile("Texture/VertexColor/T_TileBlend_Normal.png");

    resourceManager->LoadFile("FBX/SM_VertexColor");
    resourceManager->LoadFile("FBX/SKM_Monster1");
    resourceManager->LoadFile("FBX/SKM_Monster2");
    resourceManager->LoadFile("FBX/SKM_Robin");
    resourceManager->LoadFile("FBX/SM_Bush_001");
    resourceManager->LoadFile("FBX/SM_Bush_002");
    resourceManager->LoadFile("FBX/SM_Trunk_001");
    resourceManager->LoadFile("FBX/SM_CastleWall");
    resourceManager->LoadFile("FBX/SM_CastleWall_Door");
    resourceManager->LoadFile("FBX/SM_CastleWall_Pillar");
    resourceManager->LoadFile("FBX/SM_Chair");
    resourceManager->LoadFile("FBX/SM_CupTower");
    resourceManager->LoadFile("FBX/SM_Fork");
    resourceManager->LoadFile("FBX/SM_GuideBook");
    resourceManager->LoadFile("FBX/SM_Hat01");
    resourceManager->LoadFile("FBX/SM_Hat02");
    resourceManager->LoadFile("FBX/SM_SmallBush_001");
    resourceManager->LoadFile("FBX/SM_Stone_001");
    resourceManager->LoadFile("FBX/SM_Stone_002");
    resourceManager->LoadFile("FBX/SM_Stump");
    resourceManager->LoadFile("FBX/SM_Temple_Book_etc");
    resourceManager->LoadFile("FBX/SM_Temple_Books");
    resourceManager->LoadFile("FBX/SM_Temple_Floor");
    resourceManager->LoadFile("FBX/SM_Temple_Pillar");
    resourceManager->LoadFile("FBX/SM_Temple_Pillar_Broken");
    resourceManager->LoadFile("FBX/SM_Temple_Rabbit");
    resourceManager->LoadFile("FBX/SM_Mushroom01");
    resourceManager->LoadFile("FBX/SM_Mushroom02");
    resourceManager->LoadFile("FBX/SM_Temple_Welcome");
    resourceManager->LoadFile("FBX/SM_Stage1_Floor");

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
		//auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		//auto camComp = camObj->AddComponent<RTSCam>();
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
    editor::InstanceManager::GetSingletonInstance().ApplyInstancesAsPlaytimeObjects();

	/// Editor 에서 수정하여 Map Data 에 저장할 부분

    /// 카메라의 경우 CameraData 의 ApplyInstancesAsPlaytimeObjects 에서 처리함
	//auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//auto camComp = camObj->AddComponent<RTSCam>();
	//camObj->GetTransform()->SetLocalPosition({ 0,25,0 });
	//camObj->AddComponent<Dotween>();
	//RegisterToEditorObjectVector(camObj);

	//auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	//auto sphereMesh = rsrcMgr->GetMesh(L"Sphere");
	//auto mouseCursorObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//RegisterToEditorObjectVector(mouseCursorObject);
	//auto mouseCursorMesh = mouseCursorObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	//mouseCursorMesh->GetGI().SetMesh(sphereMesh);
	//mouseCursorMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 0, 1 });

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
	//	{
	//		mouseCursorObject->GetTransform()->SetWorldPosition(pos);
	//		SkillPreviewSystem::Instance().SetCurrentMousPosition(pos);
	//	};

	InputManager::Instance();
	UIManager::Instance();
	//PlayerController::SingleInstance().SetMovingSystemComponent(camComp);
	//TacticModeSystem::SingleInstance().SetMovingSystemComponent(camComp);
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
		e->SetSelfActive(false);
		yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(e);
	}
	objectCreatedByEditorVector.clear();
}

void application::contents::ContentsLayer::RegisterToEditorObjectVector(GameObject* p_obj)
{
    objectCreatedByEditorVector.push_back(p_obj);
}
