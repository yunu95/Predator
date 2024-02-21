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

	{
		auto uiImageObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto uiImageComponent = uiImageObject->AddComponent<yunutyEngine::graphics::UIImage>();
		//uiImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
		auto uiButtonComponent = uiImageObject->AddComponent<UIButton>();
		uiButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
		uiButtonComponent->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/zoro_highLighted.jpg"));
		uiButtonComponent->SetClickedImage(rsrcMgr->GetTexture(L"Texture/zoro_Clicked.jpg"));
		uiButtonComponent->SetWidth(255.0f);
		uiButtonComponent->SetHeight(255.0f);
		uiButtonComponent->SetImageComponent(uiImageComponent);
		uiButtonComponent->SetLayer(100);
		uiImageObject->GetTransform()->SetLocalPosition({ 255, 255, 0 });
	}

	{
		auto uiImageObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto uiImageComponent = uiImageObject->AddComponent<yunutyEngine::graphics::UIImage>();
		//uiImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
		auto uiButtonComponent = uiImageObject->AddComponent<UIButton>();
		uiButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
		uiButtonComponent->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/zoro_highLighted.jpg"));
		uiButtonComponent->SetClickedImage(rsrcMgr->GetTexture(L"Texture/zoro_Clicked.jpg"));
		uiButtonComponent->SetWidth(255.0f);
		uiButtonComponent->SetHeight(255.0f);
		uiButtonComponent->SetImageComponent(uiImageComponent);
		uiButtonComponent->SetLayer(90);
		uiImageObject->GetTransform()->SetLocalPosition({ 125.5f, 255, 0 });
	}

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
