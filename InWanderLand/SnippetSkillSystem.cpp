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
#include "ProjectileSystem.h"
#include "PlayerController.h"
#include "UnitFactory.h"
#include "InputManager.h"
#include "StatusTimerPool.h"
#include "WarriorProduction.h"
#include "HealerProduction.h"
#include "MagicianProduction.h"
#include "MeleeEnemyProduction.h"
#include "UnitProductionOrder.h"

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

	auto sphereMesh = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetMesh(L"Sphere");
	auto mouseCursorObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto mouseCursorMesh = mouseCursorObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	mouseCursorMesh->GetGI().SetMesh(sphereMesh);
	mouseCursorMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 0, 0 });

	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();

	auto rtsCam = camObj->AddComponent<RTSCam>();
	rtsCam->groundHoveringClickCallback = [=](Vector3d pos)
	{
		mouseCursorObject->GetTransform()->SetWorldPosition(pos);
	};
	std::vector<Vector3f> worldVertices{ };
	std::vector<int> worldFaces{ };

	camObj->GetTransform()->SetWorldPosition(Vector3d(0, 30, 0));

	// 길찾기 필드 생성
	
	//int bottomLeftX = 0;
	//int bottomLeftZ = 0;
	//int topRightX = 100;
	//int topRightZ = 100;
	//for (int i = 0; i < 5; i++)
	//{
	//	bottomLeftX += i * 100;
	//	topRightX += i * 100;
	//	for (int j = 0; j < 5; j++)
	//	{
	//		bottomLeftZ += j * 100;
	//		topRightZ += j * 100;
	//		Vector3d bottomLeft = Vector3d(bottomLeftX, 0, bottomLeftZ);
	//		Vector3d topRight = Vector3d(topRightX, 0, topRightZ);
	//		CreateNavigationPlanes(bottomLeft, topRight, worldVertices, worldFaces);
	//	}
	//}
	//CreateNavigationPlanes({ -500,0,-500 }, { 500,0,500 }, worldVertices, worldFaces);
	CreateNavigationPlanes({ -8,0,-2 }, { 8,0,2 }, worldVertices, worldFaces);
	CreateNavigationPlanes({ -8,0,-8 }, { -6,0,8 }, worldVertices, worldFaces);
	CreateNavigationPlanes({ 6,0,-8 }, { 8,0,8 }, worldVertices, worldFaces);
	CreateNavigationPlanes({ -8,0,6 }, { 8,0,8 }, worldVertices, worldFaces);
	CreateNavigationPlanes({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
	//CreateNavigationPlanes({ -1000,0,-1000 }, { 1000,0,1000 }, worldVertices, worldFaces);
	auto fieldObject = Scene::getCurrentScene()->AddGameObject();

	auto navField = fieldObject->AddComponent<yunutyEngine::NavigationField>();
	navField->BuildField(worldVertices, worldFaces);

	/// ProjectileSystem SetUp
	ProjectileSystem::GetInstance()->SetUp();

	/// PlayerController SetUp
	PlayerController::GetInstance()->SetMovingSystemComponent(rtsCam);

	/// UnitFactory
	unique_ptr<UnitFactory> unitfactory = make_unique<UnitFactory>();

	/// StatusManager 
	StatusTimerPool::GetInstance()->SetUp();

	/// FBX File Load
	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	//rsrcManager->LoadFile("FBX/Player");
	rsrcManager->LoadFile("FBX/Boss");

	/// Player1 : Warrior Unit Production
	//unique_ptr<WarriorProduction> warriorProductor = make_unique<WarriorProduction>();
	auto warriorProductor = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<WarriorProduction>();
	warriorProductor->SetUnitData(yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Boss"), navField, Vector3d(-7.0f, 0.0f, 0.0f));
	auto player1GameObject = warriorProductor->CreateUnitWithOrder();

	auto magicianProductor = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<MagicianProduction>();
	magicianProductor->SetUnitData(yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Boss"), navField, Vector3d(-7.0f, 0.0f, 7.0f));
	auto player2GameObject = magicianProductor->CreateUnitWithOrder();

	auto healerProductor = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<HealerProduction>();
	healerProductor->SetUnitData(yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Boss"), navField, Vector3d(-7.0f, 0.0f, -7.0f));
	auto player3GameObject = healerProductor->CreateUnitWithOrder();

	auto meleeEnemyProductor1 = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<MeleeEnemyProduction>();
	meleeEnemyProductor1->SetUnitData(yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Boss"), navField, Vector3d(7.0f, 0.0f, 7.0f));
	auto enemy1GameObject = meleeEnemyProductor1->CreateUnitWithOrder();
	/// Test - Make Cube which doesn't have Kinematic Collider 
	//graphics::StaticMeshRenderer* debugMesh;
	//auto cubeObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//cubeObject->GetTransform()->SetWorldPosition({ 7,3,0 });
	//debugMesh = AttachDebugMesh(cubeObject, DebugMeshType::Cube, yunuGI::Color::green(), false);

	//auto cubeCollider = cubeObject->AddComponent<physics::BoxCollider>();
	//cubeCollider->SetHalfExtent({ 1,1,1 });
	//auto cubeRigidBody = cubeObject->AddComponent<physics::RigidBody>();
	//cubeRigidBody->SetAsKinematic(false);


	{
		auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
		directionalLight->GetTransform()->SetWorldPosition(Vector3d(0, 50, 0));
		directionalLight->GetTransform()->rotation = Quaternion{ Vector3d{100,0,0} };
	}

	auto inputManagerObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	inputManagerObject->AddComponent<InputManager>();

	yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace snippets
{
	TEST_CLASS(InWanderLand)
	{
	public:
		TEST_METHOD(SnippetSkillSystem)
		{
			application::Application& client = application::Application::CreateApplication(0, 0);
			application::Contents::ContentsLayer::AssignTestInitializer(SnippetSkillSystemInit);
			client.Initialize();
			client.Run();
			client.Finalize();
		}
	};
}