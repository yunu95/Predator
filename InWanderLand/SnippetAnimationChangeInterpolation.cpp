#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "TestComponent2.h"
#include "RTSCam.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void SnippetAnimationChangeInterpolationInit()
{
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

	{
		auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
		directionalLight->GetTransform()->rotation = Quaternion{ Vector3d{90,0,45} };
	}

	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();

	auto rtsCam = camObj->AddComponent<RTSCam>();

	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	_resourceManager->LoadFile("FBX/Boss");

	for (int i = 0; i < 1000; ++i)
	{
		{
			auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			auto tempX = static_cast<float>(rand() % 100);
			auto tempZ = static_cast<float>(rand() % 100);
			object->GetTransform()->position = Vector3d{ tempX,0,tempZ };
			auto renderer = object->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
			auto meshList = _resourceManager->GetMeshList();
			for (auto& i : meshList)
			{
				if (i->GetName() == L"Sphere")
				{
					renderer->GetGI().SetMesh(i);
				}
			}
		}
	}

	yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace snippets
{
	TEST_CLASS(InWanderLand)
	{
	public:
		// 테스트 함수의 이름이 Snippet으로 시작하는 테스트들은 빌드의 성공 여부 판단에 쓰이지 않습니다.
		// RunTests.bat를 실행해도 이 테스트들은 실행되지 않으며, Jenkins에서도 이 테스트들은 실행되지 않습니다.
		// 이 테스트들은 오직 개발자가 직접 엔트리 포인트를 달리하여 테스트를 진행하고 싶을 때의 용도로만 사용됩니다.
		TEST_METHOD(SnippetAnimationChangeInterpolation)
		{
			application::Application& client = application::Application::CreateApplication(0, 0);
			application::Contents::ContentsLayer::AssignTestInitializer(SnippetAnimationChangeInterpolationInit);
			client.Initialize();
			client.Run();
			client.Finalize();
		}
	};
}
#endif