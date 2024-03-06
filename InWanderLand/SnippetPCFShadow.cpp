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
void SnippetInitializerPCFShadow()
{
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	
	auto& meshList = rsrcManager->GetMeshList();
	yunuGI::IMesh* quadMesh;
	yunuGI::IMesh* sphereMesh;
	for (auto& i : meshList)
	{
		if (i->GetName() == L"Rectangle")
		{
			quadMesh = i;
		}
		if (i->GetName() == L"Sphere")
		{
			sphereMesh = i;
		}
	}

	{
		auto obj = Scene::getCurrentScene()->AddGameObject();
		obj->GetTransform()->SetLocalScale(Vector3d{ 3,3,1 });
		auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		renderer->GetGI().SetMesh(quadMesh);
	}

	{
		auto obj = Scene::getCurrentScene()->AddGameObject();
		obj->GetTransform()->SetLocalPosition(Vector3d{ 0,0,-2 });
		auto renderer = obj->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		renderer->GetGI().SetMesh(sphereMesh);
	}

	auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
	//directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 90,0,0 }));
	directionalLight->GetTransform()->SetLocalPosition(Vector3d{0,0,-10});

	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<tests::GraphicsTestCam>();

	camObj->GetTransform()->SetWorldPosition({ 0,5,-15 });

}

namespace snippets
{
	TEST_CLASS(SnippetPCFShadowClass)
	{
	public:
		// 테스트 함수의 이름이 Snippet으로 시작하는 테스트들은 빌드의 성공 여부 판단에 쓰이지 않습니다.
		// RunTests.bat를 실행해도 이 테스트들은 실행되지 않으며, Jenkins에서도 이 테스트들은 실행되지 않습니다.
		// 이 테스트들은 오직 개발자가 직접 엔트리 포인트를 달리하여 테스트를 진행하고 싶을 때의 용도로만 사용됩니다.
		TEST_METHOD(SnippetPCFShadw)
		{
			application::Application& client = application::Application::CreateApplication(0, 0);
			application::contents::ContentsLayer::AssignTestInitializer(SnippetInitializerPCFShadow);
			client.Initialize();
			client.Run();
			client.Finalize();
		}
	};
}
#endif
