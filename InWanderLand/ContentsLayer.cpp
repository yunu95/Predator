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
	camObj->GetTransform()->SetWorldPosition(Vector3d(-10, 10, 0));
	camObj->GetTransform()->SetWorldRotation(Vector3d(115, 145, 0));

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

	/// 투사체 objectPool 셋업
	ProjectileSystem::GetInstance()->SetUp();

	UnitFactory* factory = new UnitFactory;

	auto playerGameObject = factory->CreateUnit(UnitType::PLAYER, navField, Vector3d(5.0f, 0.0f, 0.0f));
	playerGameObject->GetComponent<Player>()->SetMovingSystemComponent(rtsCam);

	auto enemyGameObject = factory->CreateUnit(UnitType::ENEMY, navField, Vector3d(-5.0f, 0.0f, 0.0f));

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
