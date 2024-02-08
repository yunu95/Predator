#include "InWanderLand.h"
#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "TestComponent2.h"
#include "TestUtilGraphicsTestCam.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void SnippetCastleWallTextureBlendingInit()
{
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

	{
		auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
		directionalLight->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{90,0,45} });
	}

	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	camObj->AddComponent<tests::GraphicsTestCam>();
	camObj->GetTransform()->SetLocalPosition(Vector3d{ 0,4,-15 });

	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	_resourceManager->LoadFile("FBX/SM_CastleWall");
	{
		auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_CastleWall");
	}


	yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace snippets
{

	TEST_CLASS(SnippetCastleWallTextureBlendingClass)
	{
	public:
		// TestComponent2안에 H키와 J키를 눌렀을 때 애니메이션 전환하게 되어있음.
		TEST_METHOD(SnippetCastleWallTextureBlending)
		{
			application::Application& client = application::Application::CreateApplication(0, 0);
			application::contents::ContentsLayer::AssignTestInitializer(SnippetCastleWallTextureBlendingInit);
			client.Initialize();
			client.Run();
			client.Finalize();
		}
	};
}
#endif
