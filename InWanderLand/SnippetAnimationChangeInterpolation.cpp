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

	for (int i = 0; i < 1; ++i)
	{
		for (int j = 0; j < 1; ++j)
		{
			float temp = 2 * i;
			float temp2 = 2 * j;
			auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Boss");
			auto test = object->AddComponent<TestComponent2>();
			object->GetTransform()->position = Vector3d{ temp,temp2,0 };
			auto animator = object->GetComponent<yunutyEngine::graphics::Animator>();
			test->anim = animator;
			auto& animationList = _resourceManager->GetAnimationList();
			for (auto& i : animationList)
			{
				if (i->GetName() == L"root|000.Idle")
				{
					test->idleAnimation = i;
					i->SetPlaySpeed(2.f);
					i->SetLoop(true);
					animator->GetGI().PushAnimation(i);
					animator->GetGI().Play(i);
				}

				if (i->GetName() == L"root|001-2.Walk")
				{
					test->walkAnimation = i;
					i->SetPlaySpeed(0.5f);
					i->SetLoop(true);
					animator->GetGI().PushAnimation(i);
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