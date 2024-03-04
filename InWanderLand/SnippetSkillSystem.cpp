#include "InWanderLand.h"
#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "Application.h"
#include "RTSCam.h"
#include "DelayedTestFunctions.h"
#include "ShakyCam.h"
#include "DebugTilePlane.h"
#include "DebugBeacon.h"
#include "DebugMeshes.h"
#include "DebuggingMeshPool.h"
#include "PlayerController.h"
#include "UnitFactory.h"
#include "InputManager.h"
#include "StatusTimerPool.h"
#include "WarriorProductor.h"
#include "HealerProductor.h"
#include "MagicianProductor.h"
#include "MeleeEnemyProductor.h"
#include "UnitProductor.h"
#include "TacticModeSystem.h"
#include "SingleNavigationField.h"
#include "SkillPreviewSystem.h"
#include "UIButton.h"
#include "UIManager.h"
#include "SingletonUpdate.h"
#include "UIPanel.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void CreateNavigationPlanes(Vector3f botleft, Vector3f topright, std::vector<Vector3f>& worldVertices, std::vector<int>& worldFaces)
{
	int startingIdx = worldVertices.size();
	worldVertices.push_back({ botleft.x,0,topright.z });
	worldVertices.push_back({ botleft.x,0,botleft.z });
	worldVertices.push_back({ topright.x,0,botleft.z });
	worldVertices.push_back({ topright.x,0,topright.z });

	worldFaces.push_back(startingIdx + 2);
	worldFaces.push_back(startingIdx + 1);
	worldFaces.push_back(startingIdx + 0);
	worldFaces.push_back(startingIdx + 3);
	worldFaces.push_back(startingIdx + 2);
	worldFaces.push_back(startingIdx + 0);

	auto tilePlane = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DebugTilePlane>();
	auto size = topright - botleft;
	tilePlane->GetTransform()->SetWorldPosition((botleft + topright) / 2.0);
	tilePlane->width = size.x;
	tilePlane->height = size.z;
	tilePlane->SetTiles();
}

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void SnippetSkillSystemInit()
{
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
	yunutyEngine::Collider2D::SetIsOnXYPlane(false);

	auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	auto sphereMesh = rsrcMgr->GetMesh(L"Sphere");
	auto mouseCursorObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto mouseCursorMesh = mouseCursorObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	mouseCursorMesh->GetGI().SetMesh(sphereMesh);
	mouseCursorMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 0, 1 });

	auto skillPreviewCubeMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(skillPreviewCubeMeshObject, DebugMeshType::Cube)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
	SkillPreviewSystem::SingleInstance().SetPathPreviewObject(skillPreviewCubeMeshObject);

	auto skillPreviewSphereMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(skillPreviewSphereMeshObject, DebugMeshType::Sphere)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
	SkillPreviewSystem::SingleInstance().SetRangePreviewObject(skillPreviewSphereMeshObject);

	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();

	auto rtsCam = camObj->AddComponent<RTSCam>();
	rtsCam->groundHoveringClickCallback = [=](Vector3d pos)
	{
		mouseCursorObject->GetTransform()->SetWorldPosition(pos);
		SkillPreviewSystem::SingleInstance().SetCurrentMousPosition(pos);
	};
	std::vector<Vector3f> worldVertices{ };
	std::vector<int> worldFaces{ };

	camObj->GetTransform()->SetWorldPosition(Vector3d(0, 30, 0));

	CreateNavigationPlanes({ -30,0,-30 }, { 30,0,30 }, worldVertices, worldFaces);

	//CreateNavigationPlanes({ -1000,0,-1000 }, { 1000,0,1000 }, worldVertices, worldFaces);
	auto fieldObject = Scene::getCurrentScene()->AddGameObject();

	SingleNavigationField::Instance().BuildField(worldVertices, worldFaces);

	/// PlayerController SetUp
	PlayerController::GetInstance()->SetMovingSystemComponent(rtsCam);
	TacticModeSystem::SingleInstance().SetMovingSystemComponent(rtsCam);

	WarriorProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, 0.0f));;
	MagicianProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, -7.0f));;
	HealerProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, 7.0f));
	MeleeEnemyProductor::Instance().CreateUnit(Vector3d(7.0f, 0.0f, 7.0f));
	MeleeEnemyProductor::Instance().CreateUnit(Vector3d(7.0f, 0.0f, -7.0f));

	/// UIButton Test
	rsrcMgr->LoadFile("Texture/zoro.jpg");
	rsrcMgr->LoadFile("Texture/zoro_highLighted.jpg");
	rsrcMgr->LoadFile("Texture/zoro_Clicked.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Menu.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/TacticMode.jpg");

	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Robin_ParentUI.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Ursula_ParentUI.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Gretel_ParentUI.jpg");

	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Robin_Portrait.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Ursula_Portrait.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Gretel_Portrait.jpg");

	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Robin_Skill1_Ikon.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Ursula_Skill1_Ikon.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Gretel_Skill1_Ikon.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Robin_Skill2_Ikon.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Ursula_Skill2_Ikon.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Gretel_Skill2_Ikon.jpg");

	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/CloseButton.jpg");

	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/menu_window.jpg");


#pragma region Menu UI
	auto menuPanel = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<UIPanel>();

	auto menuWindowUIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto menuWindowImageComponent = menuWindowUIObject->AddComponent<yunutyEngine::graphics::UIImage>();
	menuWindowImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/menu_window.jpg"));
	menuWindowImageComponent->GetGI().SetLayer(1);

	auto menuUIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto menuUIImageComponent = menuUIObject->AddComponent<yunutyEngine::graphics::UIImage>();
	auto menuUIButtonComponent = menuUIObject->AddComponent<UIButton>();
	menuUIButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Menu.jpg"));
	//menuUIButtonComponent->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/zoro_highLighted.jpg"));
	//menuUIButtonComponent->SetClickedImage(rsrcMgr->GetTexture(L"Texture/zoro_Clicked.jpg"));
	menuUIButtonComponent->SetImageComponent(menuUIImageComponent);
	menuUIButtonComponent->SetLayer(100);
	menuUIObject->GetTransform()->SetLocalPosition({ 30, 1000, 0 });
	menuUIButtonComponent->SetButtonClickFunction([=]()
		{

		});
#pragma endregion

	auto uiImageObject2 = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto uiImageComponent2 = uiImageObject2->AddComponent<yunutyEngine::graphics::UIImage>();
	auto uiButtonComponent2 = uiImageObject2->AddComponent<UIButton>();
	uiButtonComponent2->SetIdleImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
	uiButtonComponent2->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/zoro_highLighted.jpg"));
	uiButtonComponent2->SetClickedImage(rsrcMgr->GetTexture(L"Texture/zoro_Clicked.jpg"));
	uiButtonComponent2->SetImageComponent(uiImageComponent2);
	uiButtonComponent2->SetLayer(90);
	uiButtonComponent2->SetCloseButton();
	uiImageObject2->GetTransform()->SetLocalPosition({ 125.5f, 255, 0 });




	//auto closeButtonObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//auto closeButtonUiImageComponent = closeButtonObj->AddComponent<yunutyEngine::graphics::UIImage>();
	//auto closeButtonComponent = closeButtonObj->AddComponent<UIButton>();
	//closeButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/CloseButton.jpg"));
	//closeButtonComponent->SetImageComponent(closeButtonUiImageComponent);
	//closeButtonComponent->SetLayer(91);
	//closeButtonObj->SetParent(uiImageObject2);
	//closeButtonComponent->SetButtonClickFunction([=]()
	//	{
	//		closeButtonObj->GetParentGameObject()->SetSelfActive(false);
	//	});
	//closeButtonObj->GetTransform()->SetLocalPosition({ 112.5f, 112.5f, 0.0f });




	//{
	//	auto uiImageObject2 = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//	auto uiImageComponent2 = uiImageObject2->AddComponent<yunutyEngine::graphics::UIImage>();
	//	auto uiButtonComponent2 = uiImageObject2->AddComponent<UIButton>();
	//	uiButtonComponent2->SetIdleImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
	//	uiButtonComponent2->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/zoro_highLighted.jpg"));
	//	uiButtonComponent2->SetClickedImage(rsrcMgr->GetTexture(L"Texture/zoro_Clicked.jpg"));
	//	uiButtonComponent2->SetImageComponent(uiImageComponent2);
	//	uiButtonComponent2->SetLayer(80);
	//	uiImageObject2->GetTransform()->SetLocalPosition({ 10, 255, 0 });
	//}

	UIManager::SingleInstance().SetRTSCam(rtsCam);

	auto singletonUpdateComponent = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<SingletonUpdate>();

	/// UItext Test
	//auto enemy1TextObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//auto enemy1UITextCom = enemy1TextObj->AddComponent<graphics::UIText>();
	//enemy1TextObj->SetParent(player1GameObject);
	//enemy1TextObj->GetTransform()->SetLocalScale(Vector3d(1000, 1000, 0));
	//enemy1UITextCom->GetGI().SetFontSize(76);
	//enemy1UITextCom->GetGI().SetText(L"Test");
	{
		auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
		directionalLight->GetTransform()->SetWorldPosition(Vector3d(0, 100, 0));
		directionalLight->GetTransform()->SetLocalRotation( Quaternion{ Vector3d{120,0,0} });
	}
	yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace snippets
{
	TEST_CLASS(SnippetSkillSystemClass)
	{
	public:
		TEST_METHOD(SnippetSkillSystem)
		{
			application::Application& client = application::Application::CreateApplication(0, 0);
			application::contents::ContentsLayer::AssignTestInitializer(SnippetSkillSystemInit);
			client.Initialize();
			client.Run();
			client.Finalize();
		}
	};
}
#endif
