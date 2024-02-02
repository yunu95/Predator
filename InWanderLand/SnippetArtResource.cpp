#include "InWanderLand.h"
#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "TestUtilGraphicsTestCam.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void SnippetInitializerArtResource()
{
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	
#pragma region Cup Tower
	rsrcManager->LoadFile("FBX/CupTower");
	{
		auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("CupTower");
	}
#pragma endregion

#pragma region Hat01
	rsrcManager->LoadFile("FBX/Hat01");
	{
		auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("Hat01");
		obj->GetTransform()->SetLocalPosition(Vector3d{ 7,0,0 });
	}
#pragma endregion

#pragma region Hat02
	rsrcManager->LoadFile("FBX/Hat02");
	{
		auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("Hat02");
		obj->GetTransform()->SetLocalPosition(Vector3d{ 14,0,0 });
	}
#pragma endregion

#pragma region Stump
	rsrcManager->LoadFile("FBX/Stump");
	{
		auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("Stump");
		obj->GetTransform()->SetLocalPosition(Vector3d{ 30,0,0 });
	}
#pragma endregion

#pragma region CastleWall_Door
	rsrcManager->LoadFile("FBX/CastleWall_Door");
	{
		auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("CastleWall_Door");
		obj->GetTransform()->SetLocalPosition(Vector3d{ 50,0,0 });
	}
#pragma endregion

#pragma region Monster1
	//rsrcManager->LoadFile("FBX/Monster1");
	//{
	//	auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("Monster1");
	//	obj->GetTransform()->SetLocalPosition(Vector3d{ 60,0,0 });
	//}
#pragma endregion


#pragma region Monster2
	//rsrcManager->LoadFile("FBX/Monster2");
	//{
	//	auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("Monster2");
	//	obj->GetTransform()->SetLocalPosition(Vector3d{ 70,0,0 });
	//}
#pragma endregion


	auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
	//directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 90,0,0 }));
	directionalLight->GetTransform()->SetLocalPosition(Vector3d{0,0,-10});

	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<tests::GraphicsTestCam>();

	camObj->GetTransform()->SetWorldPosition({ 0,5,-15 });

	yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace snippets
{
	TEST_CLASS(SnippetArtResourceClass)
	{
	public:
		// 테스트 함수의 이름이 Snippet으로 시작하는 테스트들은 빌드의 성공 여부 판단에 쓰이지 않습니다.
		// RunTests.bat를 실행해도 이 테스트들은 실행되지 않으며, Jenkins에서도 이 테스트들은 실행되지 않습니다.
		// 이 테스트들은 오직 개발자가 직접 엔트리 포인트를 달리하여 테스트를 진행하고 싶을 때의 용도로만 사용됩니다.
		TEST_METHOD(SnippetArtResource)
		{
			application::Application& client = application::Application::CreateApplication(0, 0);
			application::contents::ContentsLayer::AssignTestInitializer(SnippetInitializerArtResource);
			client.Initialize();
			client.Run();
			client.Finalize();
		}
	};
}
#endif
