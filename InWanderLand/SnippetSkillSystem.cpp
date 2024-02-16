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
		SkillPreviewSystem::SingleInstance().Update();
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
	//CreateNavigationPlanes({ -8,0,-2 }, { 8,0,2 }, worldVertices, worldFaces);
	//CreateNavigationPlanes({ -8,0,-8 }, { -6,0,8 }, worldVertices, worldFaces);
	//CreateNavigationPlanes({ 6,0,-8 }, { 8,0,8 }, worldVertices, worldFaces);
	//CreateNavigationPlanes({ -8,0,6 }, { 8,0,8 }, worldVertices, worldFaces);
	//CreateNavigationPlanes({ -2,0,-8 }, { 2,0,8 }, worldVertices, worldFaces);
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

	auto inputManagerObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	inputManagerObject->AddComponent<InputManager>();

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
