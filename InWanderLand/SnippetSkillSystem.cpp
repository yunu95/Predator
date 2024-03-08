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

class StalkingUI : public Component
{
	GameObject* soundWindowObject;
	virtual void Start() override
	{
		auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		auto soundPanel = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<UIPanel>();
		soundWindowObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto soundWindowImageComponent = soundWindowObject->AddComponent<yunutyEngine::graphics::UIImage>();
		soundWindowImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Sound_Window.jpg"));
		soundWindowImageComponent->GetGI().SetLayer(6974892);
	}
	virtual void Update()
	{
		auto mouseScreenPos = Input::getMouseScreenPositionNormalized();
		mouseScreenPos.x *= 1920;
		mouseScreenPos.y *= 1080;
		soundWindowObject->GetTransform()->SetWorldPosition({ mouseScreenPos.x, mouseScreenPos.y, 0 });
	}
};
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

	Scene::getCurrentScene()->AddGameObject()->AddComponent<StalkingUI>();
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
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/GameManual_Button.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/GameManual.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Sound_Window.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Sound_Button.jpg");
	rsrcMgr->LoadFile("Texture/UI/InGameUITemp/Restart_Button.jpg");


	const int inGameLayer = 1;

	auto menuPanel = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<UIPanel>();
	//SoundSystem::PlayMusic("Texture/UI/InGameUITemp/Game_BGM.mp3");

#pragma region MenuButton
	auto menuUIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto menuUIImageComponent = menuUIObject->AddComponent<yunutyEngine::graphics::UIImage>();
	auto menuUIButtonComponent = menuUIObject->AddComponent<UIButton>();
	menuUIButtonComponent->SetImageComponent(menuUIImageComponent);
	menuUIButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Menu.jpg"));
	menuUIButtonComponent->SetLayer(100);
	menuUIButtonComponent->SetButtonClickFunction([=]()
		{
			menuPanel->SetPanelActive(true);
			SoundSystem::PlaySoundfile("Texture/UI/InGameUITemp/TestClick2.wav");
			SoundSystem::PlaySoundfile("Texture/UI/InGameUITemp/TestClick.wav");
		});
	menuUIObject->GetTransform()->SetLocalPosition({ 0, 1000, 0 });
#pragma endregion

	const int penalLayer = 10;
	//const int buttonLayer = 11;

#pragma region Menu Window
	const int menuWindowXpos = 760;
	const int menuWindowYpos = 540 - 350 / 2;

	auto menuWindowUIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto menuWindowImageComponent = menuWindowUIObject->AddComponent<yunutyEngine::graphics::UIImage>();
	menuWindowImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/menu_window.jpg"));
	menuWindowImageComponent->GetGI().SetLayer(penalLayer);
	menuWindowUIObject->SetSelfActive(false);
	menuWindowUIObject->GetTransform()->SetWorldPosition({ menuWindowXpos, menuWindowYpos, 0 });
	menuPanel->SetWindowImage(menuWindowImageComponent);
#pragma endregion

#pragma region Game Manual Panel
	auto gameManualPanel = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<UIPanel>();
	auto gameManualWindowObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto gameManualWindowImageComponent = gameManualWindowObject->AddComponent<yunutyEngine::graphics::UIImage>();
	gameManualWindowImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/GameManual.jpg"));
	gameManualWindowImageComponent->GetGI().SetLayer(penalLayer);
	gameManualWindowObject->SetSelfActive(false);
	gameManualWindowObject->GetTransform()->SetWorldPosition({ menuWindowXpos, menuWindowYpos, 0 });
	gameManualPanel->SetWindowImage(gameManualWindowImageComponent);
#pragma endregion

#pragma region Game Manual Button
	{
		auto gameManualButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto gameManualImageComponent = gameManualButtonObject->AddComponent<yunutyEngine::graphics::UIImage>();
		auto gameManualButtonComponent = gameManualButtonObject->AddComponent<UIButton>();
		gameManualButtonComponent->SetImageComponent(gameManualImageComponent);
		gameManualButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/GameManual_Button.jpg"));
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
	auto soundWindowImageComponent = soundWindowObject->AddComponent<yunutyEngine::graphics::UIImage>();
	soundWindowImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Sound_Window.jpg"));
	soundWindowImageComponent->GetGI().SetLayer(penalLayer);
	soundWindowObject->SetSelfActive(false);
	soundWindowObject->GetTransform()->SetWorldPosition({ menuWindowXpos, menuWindowYpos, 0 });
	soundPanel->SetWindowImage(soundWindowImageComponent);
#pragma endregion

#pragma region Sound Button
	{
		auto soundButtonObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto soundButtonImageComponent = soundButtonObject->AddComponent<yunutyEngine::graphics::UIImage>();
		auto soundButtonComponent = soundButtonObject->AddComponent<UIButton>();
		soundButtonComponent->SetImageComponent(soundButtonImageComponent);
		soundButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Sound_Button.jpg"));
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
		auto restartButtonImageComponent = restartButtonObject->AddComponent<yunutyEngine::graphics::UIImage>();
		auto restartButtonComponent = restartButtonObject->AddComponent<UIButton>();
		restartButtonComponent->SetImageComponent(restartButtonImageComponent);
		restartButtonComponent->SetIdleImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Restart_Button.jpg"));
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
	auto tacticModeImageComponent = tacticModeUIObject->AddComponent<yunutyEngine::graphics::UIImage>();
	tacticModeImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/TacticMode.jpg"));
	tacticModeImageComponent->GetGI().SetLayer(inGameLayer);
	tacticModeUIObject->GetTransform()->SetWorldPosition({ 420 - 150 / 2, 1000 - 150 / 2, 0 });
#pragma endregion

#pragma region Robin State
	const int robinStateBarPosX = 656 - 300 / 2;
	const int robinStateBarPosY = 908 - 20 / 2;

	{
		auto robinStateBarObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto robinStateBarImageComponent = robinStateBarObject->AddComponent<yunutyEngine::graphics::UIImage>();
		robinStateBarImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Robin_ParentUI.jpg"));
		robinStateBarImageComponent->GetGI().SetLayer(inGameLayer);
		robinStateBarObject->GetTransform()->SetWorldPosition({ robinStateBarPosX, robinStateBarPosY, 0 });
	}

	{
		auto robinPortraitObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto robinPortraitImageComponent = robinPortraitObject->AddComponent<yunutyEngine::graphics::UIImage>();
		robinPortraitImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Robin_Portrait.jpg"));
		robinPortraitImageComponent->GetGI().SetLayer(inGameLayer);
		robinPortraitObject->GetTransform()->SetWorldPosition({ robinStateBarPosX + 50 - 50, robinStateBarPosY + 73 - 50, 0 });
	}

	{
		auto robinSkill1UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto robinSkill1UIImageComponent = robinSkill1UIObject->AddComponent<yunutyEngine::graphics::UIImage>();
		robinSkill1UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Robin_Skill1_Ikon.jpg"));
		robinSkill1UIImageComponent->GetGI().SetLayer(inGameLayer);
		robinSkill1UIObject->GetTransform()->SetWorldPosition({ robinStateBarPosX + 150 - 50, robinStateBarPosY + 73 - 50, 0 });
	}

	{
		auto robinSkill2UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto robinSkill2UIImageComponent = robinSkill2UIObject->AddComponent<yunutyEngine::graphics::UIImage>();
		robinSkill2UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Robin_Skill2_Ikon.jpg"));
		robinSkill2UIImageComponent->GetGI().SetLayer(inGameLayer);
		robinSkill2UIObject->GetTransform()->SetWorldPosition({ robinStateBarPosX + 250 - 50, robinStateBarPosY + 73 - 50, 0 });
	}
#pragma endregion

#pragma region Ursula State
	const int ursulaStateBarPosX = 1006 - 300 / 2;
	const int ursulaStateBarPosY = 908 - 20 / 2;

	{
		auto ursulaStateBarObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto ursulaStateBarImageComponent = ursulaStateBarObject->AddComponent<yunutyEngine::graphics::UIImage>();
		ursulaStateBarImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Ursula_ParentUI.jpg"));
		ursulaStateBarImageComponent->GetGI().SetLayer(inGameLayer);
		ursulaStateBarObject->GetTransform()->SetWorldPosition({ ursulaStateBarPosX, ursulaStateBarPosY, 0 });
	}

	{
		auto ursulaPortraitObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto ursulaPortraitImageComponent = ursulaPortraitObject->AddComponent<yunutyEngine::graphics::UIImage>();
		ursulaPortraitImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Ursula_Portrait.jpg"));
		ursulaPortraitImageComponent->GetGI().SetLayer(inGameLayer);
		ursulaPortraitObject->GetTransform()->SetWorldPosition({ ursulaStateBarPosX + 50 - 50, ursulaStateBarPosY + 73 - 50, 0 });
	}

	{
		auto ursulaSkill1UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto ursulaSkill1UIImageComponent = ursulaSkill1UIObject->AddComponent<yunutyEngine::graphics::UIImage>();
		ursulaSkill1UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Ursula_Skill1_Ikon.jpg"));
		ursulaSkill1UIImageComponent->GetGI().SetLayer(inGameLayer);
		ursulaSkill1UIObject->GetTransform()->SetWorldPosition({ ursulaStateBarPosX + 150 - 50, ursulaStateBarPosY + 73 - 50, 0 });
	}

	{
		auto ursulaSkill2UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto ursulaSkill2UIImageComponent = ursulaSkill2UIObject->AddComponent<yunutyEngine::graphics::UIImage>();
		ursulaSkill2UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Ursula_Skill2_Ikon.jpg"));
		ursulaSkill2UIImageComponent->GetGI().SetLayer(inGameLayer);
		ursulaSkill2UIObject->GetTransform()->SetWorldPosition({ ursulaStateBarPosX + 250 - 50, ursulaStateBarPosY + 73 - 50, 0 });
	}
#pragma endregion

#pragma region Gretel State
	const int gretelStateBarPosX = 1357 - 300 / 2;
	const int gretelStateBarPosY = 908 - 20 / 2;

	{
		auto gretelStateBarObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto gretelStateBarImageComponent = gretelStateBarObject->AddComponent<yunutyEngine::graphics::UIImage>();
		gretelStateBarImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Gretel_ParentUI.jpg"));
		gretelStateBarImageComponent->GetGI().SetLayer(inGameLayer);
		gretelStateBarObject->GetTransform()->SetWorldPosition({ gretelStateBarPosX, gretelStateBarPosY, 0 });
	}

	{
		auto gretelPortraitObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto gretelPortraitImageComponent = gretelPortraitObject->AddComponent<yunutyEngine::graphics::UIImage>();
		gretelPortraitImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Gretel_Portrait.jpg"));
		gretelPortraitImageComponent->GetGI().SetLayer(inGameLayer);
		gretelPortraitObject->GetTransform()->SetWorldPosition({ gretelStateBarPosX + 50 - 50, gretelStateBarPosY + 73 - 50, 0 });
	}

	{
		auto gretelSkill1UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto gretelSkill1UIImageComponent = gretelSkill1UIObject->AddComponent<yunutyEngine::graphics::UIImage>();
		gretelSkill1UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Gretel_Skill1_Ikon.jpg"));
		gretelSkill1UIImageComponent->GetGI().SetLayer(inGameLayer);
		gretelSkill1UIObject->GetTransform()->SetWorldPosition({ gretelStateBarPosX + 150 - 50, gretelStateBarPosY + 73 - 50, 0 });
	}

	{
		auto gretelSkill2UIObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto gretelSkill2UIImageComponent = gretelSkill2UIObject->AddComponent<yunutyEngine::graphics::UIImage>();
		gretelSkill2UIImageComponent->GetGI().SetImage(rsrcMgr->GetTexture(L"Texture/UI/InGameUITemp/Gretel_Skill2_Ikon.jpg"));
		gretelSkill2UIImageComponent->GetGI().SetLayer(inGameLayer);
		gretelSkill2UIObject->GetTransform()->SetWorldPosition({ gretelStateBarPosX + 250 - 50, gretelStateBarPosY + 73 - 50, 0 });
	}
#pragma endregion

	menuPanel->SetCloseButtonActive(true);
	gameManualPanel->SetCloseButtonActive(true);
	soundPanel->SetCloseButtonActive(true);

	gameManualPanel->SetParentPanel(menuPanel);
	soundPanel->SetParentPanel(menuPanel);

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
