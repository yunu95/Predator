#include "ContentsLayer.h"

#include "YunutyEngine.h"
#include "ShakyCam.h"
#include "RTSCam.h"   
#include "CamSwitcher.h"   
#include "Unit.h"
#include "PlayerController.h"
#include "Dotween.h"
#include "Projectile.h"
#include "ProjectileSystem.h"
#include "RoamingCam.h"   
#include "DebugTilePlane.h"
#include "DebugBeacon.h"
#include "RangeSystem.h"
#include "RTSCam.h"   
#include "DebugTilePlane.h"
#include "DebugBeacon.h"
#include "DebugMeshes.h"
#include "UnitFactory.h"
#include "DelayedTestFunctions.h"

#include <d3d11.h>

void CreateNavPlane(Vector3f botleft, Vector3f topright, std::vector<Vector3f>& worldVertices, std::vector<int>& worldFaces)
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

void Application::Contents::ContentsLayer::Initialize()
{
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
	yunutyEngine::Collider2D::SetIsOnXYPlane(false);

	//auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//camObj->GetTransform()->position = Vector3d(0, 0, -5);
	//auto roamingCam = camObj->AddComponent<RoamingCam>();

	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();

	auto rtsCam = camObj->AddComponent<RTSCam>();
	std::vector<Vector3f> worldVertices{ };
	std::vector<int> worldFaces{ };

	camObj->GetTransform()->SetWorldPosition(Vector3d(0, 30, 0));

	// 길찾기 필드 생성
	CreateNavPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
	CreateNavPlane({ -8,0,-2 }, { 8,0,2 }, worldVertices, worldFaces);
	CreateNavPlane({ -8,0,-8 }, { -6,0,8 }, worldVertices, worldFaces);
	CreateNavPlane({ 6,0,-8 }, { 8,0,8 }, worldVertices, worldFaces);
	CreateNavPlane({ -8,0,6 }, { 8,0,8 }, worldVertices, worldFaces);
	CreateNavPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
	auto navField = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationField>();
	navField->BuildField(worldVertices, worldFaces);

	/// 투사체 objectPool 셋업
	ProjectileSystem::GetInstance()->SetUp();

	/// Unit Members
	int playerHp = 100;
	int playerAp = 3;
	
	int enemyHp = 100;
	int enemyAp = 2;

	std::unique_ptr<UnitFactory> factory = make_unique<UnitFactory>();

	factory->SetPlayerHp(playerHp);
	factory->SetPlayerAp(playerAp);
	factory->SetEnemyHp(enemyHp);
	factory->SetEnemyAp(enemyAp);

	auto playerGameObject = factory->CreateUnit(UnitType::PLAYER, navField, Vector3d(7.0f, 0.0f, 0.0f));
	auto playerGameObject2 = factory->CreateUnit(UnitType::PLAYER, navField, Vector3d(7.0f, 0.0f, 5.0f));
	auto playerGameObject3 = factory->CreateUnit(UnitType::PLAYER, navField, Vector3d(7.0f, 0.0f, -5.0f));
	auto playerGameObject4 = factory->CreateUnit(UnitType::PLAYER, navField, Vector3d(0.0f, 0.0f, 5.0f));

	auto controllerComponent = playerGameObject->AddComponent<PlayerController>();
	auto controllerComponent2 = playerGameObject2->AddComponent<PlayerController>();
	auto controllerComponent3 = playerGameObject3->AddComponent<PlayerController>();
	auto controllerComponent4 = playerGameObject4->AddComponent<PlayerController>();
	controllerComponent->SetMovingSystemComponent(rtsCam);
	controllerComponent2->SetMovingSystemComponent(rtsCam);
	controllerComponent3->SetMovingSystemComponent(rtsCam);
	controllerComponent4->SetMovingSystemComponent(rtsCam);


	auto enemyGameObject1 = factory->CreateUnit(UnitType::ENEMY, navField, Vector3d(-7.0f, 0.0f, 0.0f));
	auto enemyGameObject2 = factory->CreateUnit(UnitType::ENEMY, navField, Vector3d(-7.0f, 0.0f, 5.0f));
	auto enemyGameObject3 = factory->CreateUnit(UnitType::ENEMY, navField, Vector3d(-7.0f, 0.0f, -5.0f));
	auto enemyGameObject4 = factory->CreateUnit(UnitType::ENEMY, navField, Vector3d(0.0f, 0.0f, -5.0f));

	{
		auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
		directionalLight->GetTransform()->SetWorldPosition(Vector3d(0, 50, 0));
		directionalLight->GetTransform()->rotation = Quaternion{ Vector3d{100,0,0} };
	}

	auto delayFunctionObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto delayFunctionComponent = delayFunctionObject->AddComponent<DelayedTestFunctions>();
	delayFunctionComponent->todoList.push_back({ 1.0f, [=]()
		{
			playerGameObject->GetComponent<Unit>()->OrderAttackMove(Vector3d::zero);
			playerGameObject2->GetComponent<Unit>()->OrderAttackMove(Vector3d::zero);
			playerGameObject3->GetComponent<Unit>()->OrderAttackMove(Vector3d::zero);
			playerGameObject4->GetComponent<Unit>()->OrderAttackMove(Vector3d::zero);

			enemyGameObject1->GetComponent<Unit>()->OrderAttackMove(Vector3d::zero);
			enemyGameObject2->GetComponent<Unit>()->OrderAttackMove(Vector3d::zero);
			enemyGameObject3->GetComponent<Unit>()->OrderAttackMove(Vector3d::zero);
			enemyGameObject4->GetComponent<Unit>()->OrderAttackMove(Vector3d::zero);
		} });


	yunutyEngine::YunutyCycle::SingleInstance().Play();
}

void Application::Contents::ContentsLayer::Update(float ts)
{

}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 

void Application::Contents::ContentsLayer::GUIProgress()
{

}

void Application::Contents::ContentsLayer::Finalize()
{

}
