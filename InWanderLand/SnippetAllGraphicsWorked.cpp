#include "InWanderLand.h"
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
void SnippetAllGraphicsWorkedInit()
{
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

	{
		auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
		directionalLight->GetTransform()->rotation = Quaternion{ Vector3d{90,0,45} };
	}

	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	camObj->AddComponent<RTSCam>();

	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	_resourceManager->LoadFile("FBX/Sponza");
	_resourceManager->LoadFile("FBX/Bush");
	_resourceManager->LoadFile("FBX/Boss");
	_resourceManager->LoadFile("FBX/BigTree");

	for (int j = 0; j < 1; ++j)
	{
		auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Boss");

		auto animator = object->GetComponent<yunutyEngine::graphics::Animator>();
		auto& animationList = _resourceManager->GetAnimationList();
		for (auto& i : animationList)
		{
			if (i->GetName() == L"root|000.Idle")
			{
				i->SetPlaySpeed(2.f);
				i->SetLoop(true);
				animator->GetGI().PushAnimation(i);
				animator->GetGI().Play(i);
			}

			if (i->GetName() == L"root|001-2.Walk")
			{
				i->SetPlaySpeed(0.5f);
				i->SetLoop(true);
				animator->GetGI().PushAnimation(i);
			}
		}
	}

	for (int i = 0; i < 500; ++i)
	{
		float tempX = static_cast<float>(rand() % 100);
		float tempZ = static_cast<float>(rand() % 100);
		auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Bush");
		object->GetTransform()->position = Vector3d{ tempX,0,tempZ };
		object->GetTransform()->rotation = Quaternion{ Vector3d{90,0,0} };
		auto object1 = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("BigTree");
		object1->GetTransform()->position = Vector3d{ tempZ,0,tempX };
		object1->GetTransform()->rotation = Quaternion{ Vector3d{90,0,0} };
	}

	{
		auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Sponza");
		object->GetTransform()->rotation = Quaternion{ Vector3d{90,0,0} };
	}

	{
		auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		object->GetTransform()->position = Vector3d{ 0,0.25,0 };
		auto light = object->AddComponent<yunutyEngine::graphics::PointLight>();
		yunuGI::Color color = yunuGI::Color{ 0,0,1,1 };
		light->GetGI().SetLightDiffuseColor(color);
		light->GetGI().SetRange(0.3);
	}

	yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace snippets
{

	TEST_CLASS(SnippetAllGraphicsWorkedClass)
	{
	public:
		// TestComponent2안에 H키와 J키를 눌렀을 때 애니메이션 전환하게 되어있음.
		TEST_METHOD(SnippetAllGraphicsWorked)
		{
			application::Application& client = application::Application::CreateApplication(0, 0);
			application::contents::ContentsLayer::AssignTestInitializer(SnippetAllGraphicsWorkedInit);
			client.Initialize();
			client.Run();
			client.Finalize();
		}
	};
}
#endif
