#include "InWanderLand.h"
#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "DebugMeshes.h"
#include "YGFactoryAdapter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void SnippetInitializerUISystem()
{
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
	auto cam = Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::Camera>();
	cam->GetTransform()->SetLocalPosition({ 0,0,-100 });

	//auto uiTexture = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunuGI::ITexture>();
	//uiTexture->SetName(L"uiTexture");

	//auto uiImageObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	//auto uiImageComponent = uiImageObject->AddComponent<yunutyEngine::graphics::UIImage>();
	//uiImageComponent->GetGI().SetImage(uiTexture);

	auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
	directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 10,10,0 }));
	yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace snippets
{
	TEST_CLASS(SnippetUISystemClass)
	{
	public:
		TEST_METHOD(SnippetUISystem)
		{
			application::Application& client = application::Application::CreateApplication(0, 0);
			application::contents::ContentsLayer::AssignTestInitializer(SnippetInitializerUISystem);
			client.Initialize();
			client.Run();
			client.Finalize();
		}
	};
}
#endif
