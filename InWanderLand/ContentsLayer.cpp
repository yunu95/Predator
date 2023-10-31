#include "ContentsLayer.h"

#include "YunutyEngine.h"
#include "ShakyCam.h"
#include "RTSCam.h"   
#include "CamSwitcher.h"   
#include "Player.h"
#include "Dotween.h"
#include "Projectile.h"
#include "ProjectileSystem.h"
#include "RoamingCam.h"   
#include "DebugTilePlane.h"
#include "DebugBeacon.h"
#include "RangeSystem.h"


#include <d3d11.h>

void Application::Contents::ContentsLayer::Initialize()
{
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

	auto camObj2 = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	camObj2->setName("Camera");

	camObj2->GetTransform()->position = Vector3d(0, 10, -20);
	camObj2->GetTransform()->rotation = Vector3d(0, 0, 0);
	//camObj2->GetTransform()->SetWorldRotation(Vector3d(90, 90, 0));
	// 지면의 좌클릭, 우클릭에 대한 콜백은 아래의 RTSCam 인스턴스에 등록할 수 있다.
	auto rtsCam = camObj2->AddComponent<RTSCam>();

	auto camSwitcher = camObj2->AddComponent<CamSwitcher>();
	//camSwitcher->cams.push_back(roamingCam);
	camSwitcher->cams.push_back(rtsCam);

	DebugBeacon::PlaceBeacon({ 0,1,-0.5 });

#pragma region Player
	auto playerGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	playerGameObject->setName("Player");

	auto playerMesh = playerGameObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	playerMesh->GetGI().LoadMesh("Capsule");
	playerMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 1, 1, 0 });

	auto playerComponent = playerGameObject->AddComponent<Player>();

	auto playerCollider = playerGameObject->AddComponent<CircleCollider2D>();

	playerGameObject->GetTransform()->SetWorldPosition(yunutyEngine::Vector3d{ 1, 0, 0 });

	playerGameObject->AddComponent<CircleCollider2D>();

	Dotween* dotweenComponent = playerGameObject->AddComponent<Dotween>();

	// 지면에 대한  좌클릭의 콜백 함수를 정의한다.
	rtsCam->groundLeftClickCallback = [=](Vector3d position)
	{
		// Before moving, Make player look at position.
		Vector3d distanceVector = position - playerGameObject->GetTransform()->GetWorldPosition();

		float speed = 3.0f;
		//dotweenComponent->DOMove(Vector3d(position.x,
		//	playerGameObject->GetTransform()->GetWorldPosition().y, position.z), distanceVector.Magnitude() / speed);
		dotweenComponent->DOMove(Vector3d(position), distanceVector.Magnitude() / speed);

	};

#pragma endregion

#pragma region Check Pattern Tiles
	//// 체크무늬로 xy 평면을 초기화하는 클래스다.
	auto tilePlane = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DebugTilePlane>();
	tilePlane->GetGameObject()->setName("tilePlane");
	tilePlane->width = 10;
	tilePlane->height = 10;
	tilePlane->SetTiles();
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
	ememyGameObject->GetTransform()->SetWorldPosition(yunutyEngine::Vector3d{ -10, 0, 0 });

	Dotween* enemyDotween = ememyGameObject->AddComponent<Dotween>();
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


	yunutyEngine::YunutyCycle::SingleInstance().autoRendering = false;
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
