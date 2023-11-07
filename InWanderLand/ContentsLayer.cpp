#include "ContentsLayer.h"

#include "YunutyEngine.h"
#include "ShakyCam.h"
#include "RTSCam.h"   
#include "CamSwitcher.h"   
#include "Player.h"
#include "Dotween.h"
#include "Projectile.h"
#include "ProjectileSystem.h"
#include "MoveDetector.h"
#include "RoamingCam.h"   
#include "DebugTilePlane.h"
#include "DebugBeacon.h"
#include "RangeSystem.h"
#include "RTSCam.h"   
#include "DebugTilePlane.h"
#include "DebugBeacon.h"
#include "DebugMeshes.h"
#include "UnitFactory.h"


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

NavigationAgent* CreateAgent(NavigationField* navField)
{
	auto agent = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationAgent>();
	agent->SetSpeed(2);
	agent->SetRadius(0.5);
	agent->AssignToNavigationField(navField);
	auto staticMesh = agent->GetGameObject()->AddGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	staticMesh->GetGI().LoadMesh("Capsule");
	staticMesh->GetGI().GetMaterial()->SetColor({ 0.75,0.75,0.75,0 });
	staticMesh->GetTransform()->position = Vector3d{ 0,0.5,0 };
	return agent;
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
	rtsCam->GetTransform()->position = Vector3d(0, 10, 0);

	const float corridorRadius = 3;
	std::vector<Vector3f> worldVertices{ };
	std::vector<int> worldFaces{ };

	// 길찾기 필드 생성

	CreateNavPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
	CreateNavPlane({ -8,0,-2 }, { 8,0,2 }, worldVertices, worldFaces);
	CreateNavPlane({ -8,0,-8 }, { -6,0,8 }, worldVertices, worldFaces);
	CreateNavPlane({ 6,0,-8 }, { 8,0,8 }, worldVertices, worldFaces);
	CreateNavPlane({ -8,0,6 }, { 8,0,8 }, worldVertices, worldFaces);
	CreateNavPlane({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
	auto navField = Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationField>();
	navField->BuildField(worldVertices, worldFaces);
	auto agent = CreateAgent(navField);
	auto agent2 = CreateAgent(navField);
	auto agent3 = CreateAgent(navField);
	rtsCam->groundRightClickCallback = [=](Vector3d position) 
	{
		agent->MoveTo(position);
		agent2->MoveTo(position);
		agent3->MoveTo(position);
	};


	/// 투사체 objectPool 셋업
	ProjectileSystem::GetInstance()->SetUp();

#pragma region Player
	auto playerGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	playerGameObject->setName("Player");

	auto playerMesh = playerGameObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	playerMesh->GetGI().LoadMesh("Capsule");
	playerMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 1, 1, 0 });

	auto playerComponent = playerGameObject->AddComponent<Player>();

	auto playerCollider = playerGameObject->AddComponent<CircleCollider2D>();

	playerGameObject->GetTransform()->SetWorldPosition(yunutyEngine::Vector3d{ 3, 0, 0 });

	playerGameObject->AddComponent<CircleCollider2D>();

	playerGameObject->AddComponent<Dotween>();

	auto playerNavigation = playerGameObject->AddComponent<NavigationAgent>();
	//playerNavigation->SetSpeed(10.0f);
	//playerNavigation->SetRadius(0.5);
	playerNavigation->AssignToNavigationField(navField);

	// 지면에 대한  좌클릭의 콜백 함수를 정의한다.
	rtsCam->groundLeftClickCallback = [=](Vector3d position)
	{
		// Before moving, Make player look at position.
		Vector3d distanceVector = position - playerGameObject->GetTransform()->GetWorldPosition();
		float speed = 5.0f;
		playerNavigation->MoveTo(Vector3d(position));
		//dotweenComponent->DOMove(Vector3d(position), distanceVector.Magnitude() / speed);

	};
#pragma endregion

#pragma region Player Identyfication RangeSystem
	auto playerIDRangeObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	playerIDRangeObject->setName("Player Identyfication RangeSystem");

	float playerIDRangeRadius = 3.0f;
	float playerAtkRangeRadius = 2.0f;

	auto playerIDRangeSystem = playerIDRangeObject->AddComponent<RangeSystem>();
	playerIDRangeSystem->SetUnitComponent(playerComponent);
	playerIDRangeSystem->SetIdRadius(playerIDRangeRadius);
	playerIDRangeSystem->SetAtkRadius(playerAtkRangeRadius);

	auto playerIDRangeMesh = playerIDRangeObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	playerIDRangeMesh->GetGI().LoadMesh("Sphere");
	playerIDRangeMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0.3, 1, 0 });
	playerIDRangeMesh->GetGI().GetMaterial()->SetPixelShader(L"DebugPS.cso");

	playerIDRangeObject->GetTransform()->SetWorldRotation(Vector3d(90, 0, 0));
	playerIDRangeObject->GetTransform()->scale = Vector2d(playerIDRangeRadius * 2, playerIDRangeRadius * 2);
	playerIDRangeObject->SetParent(playerGameObject);
#pragma endregion

#pragma region Enemy Unit
	auto ememyGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	ememyGameObject->setName("enemyUnit");
	auto enemyMesh = ememyGameObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();

	auto enemytempComponent = ememyGameObject->AddComponent<Unit>();

	auto enemyCollider = ememyGameObject->AddComponent<CircleCollider2D>();

	enemyMesh->GetGI().LoadMesh("Capsule");
	enemyMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 1, 0 });
	ememyGameObject->GetTransform()->SetWorldPosition(yunutyEngine::Vector3d{ -3, 0, 0 });

	auto enemyDotween = ememyGameObject->AddComponent<Dotween>();

	auto enemyNavigation = ememyGameObject->AddComponent<NavigationAgent>();
	enemyNavigation->SetSpeed(10.0f);
	enemyNavigation->SetRadius(0.5f);
	enemyNavigation->AssignToNavigationField(navField);

#pragma endregion

#pragma region Enemy Identyfication Range
	auto enemyIDRangeObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	enemyIDRangeObject->setName("enemyRange2DCollider");

	float enemyIDRangeRadius = 5.0f;
	float enemyAtkRangeRadius = 3.0f;

	auto enemyRangeSystem = enemyIDRangeObject->AddComponent<RangeSystem>();
	enemyRangeSystem->SetUnitComponent(enemytempComponent);
	enemyRangeSystem->SetIdRadius(enemyIDRangeRadius);
	enemyRangeSystem->SetAtkRadius(enemyAtkRangeRadius);

	auto enemyIDRangeMesh = enemyIDRangeObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	enemyIDRangeMesh->GetGI().LoadMesh("Sphere");
	enemyIDRangeMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 1, 0 });
	enemyIDRangeMesh->GetGI().GetMaterial()->SetPixelShader(L"DebugPS.cso");

	enemyIDRangeObject->GetTransform()->SetWorldRotation(Vector3d(90, 0, 0));
	enemyIDRangeObject->GetTransform()->scale = Vector2d(enemyIDRangeRadius * 2, enemyIDRangeRadius * 2);
	enemyIDRangeObject->SetParent(ememyGameObject);
#pragma endregion

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
