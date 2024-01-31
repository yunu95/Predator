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
void SnippetInitializerLeaves()
{
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	rsrcManager->LoadFile("FBX/Bush");
	rsrcManager->LoadFile("FBX/BigTree");
	rsrcManager->LoadFile("Texture/T_LeafBrush.png");
	rsrcManager->LoadFile("LeavesVS.cso");
	rsrcManager->LoadFile("LeavesPS.cso");

	yunuGI::IMaterial* material = rsrcManager->CreateMaterial(L"Leaves");
	yunuGI::IShader* vs = nullptr;
	yunuGI::IShader* ps = nullptr;
	yunuGI::ITexture* texture = nullptr;
	auto& shaderList = rsrcManager->GetShaderList();
	auto& textureList = rsrcManager->GetTextureList();
	for (auto& i : shaderList)
	{
		if (i->GetName() == L"LeavesVS.cso")
		{
			vs = i;
		}
		if (i->GetName() == L"LeavesPS.cso")
		{
			ps = i;
		}
	}
	for (auto& i : textureList)
	{
		if (i->GetName() == L"Texture/T_LeafBrush.png")
		{
			texture = i;
		}
	}
	material->SetVertexShader(vs);
	material->SetPixelShader(ps);
	yunuGI::Color color = {0.0863, 0.2196, 0.1255};
	material->SetColor(color);
	material->SetTexture(yunuGI::Texture_Type::Temp0, texture);
	for (int i = 0; i < 500; ++i)
	{
		float tempX = rand() % 100;
		float tempZ = rand() % 100;
		auto object = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Bush");
		Vector3d scale = object->GetTransform()->scale;
		object->GetTransform()->position = Vector3d{ tempX,0,tempZ };
		//object->GetTransform()->scale = Vector3d{scale.x*10,scale.y*10,scale.z*10};
		//object->GetTransform()->rotation = Quaternion{ Vector3d{90,0,0} };
		auto& childVec = object->GetChildren();
		auto renderer = childVec[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		renderer->GetGI().SetMaterial(0, material);
	}

	auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
	//directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 90,0,0 }));

	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<tests::GraphicsTestCam>();

	camObj->GetTransform()->SetWorldPosition({ 0,5,-15 });

	yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace snippets
{
	TEST_CLASS(SnippetLeavesClass)
	{
	public:
		// 테스트 함수의 이름이 Snippet으로 시작하는 테스트들은 빌드의 성공 여부 판단에 쓰이지 않습니다.
		// RunTests.bat를 실행해도 이 테스트들은 실행되지 않으며, Jenkins에서도 이 테스트들은 실행되지 않습니다.
		// 이 테스트들은 오직 개발자가 직접 엔트리 포인트를 달리하여 테스트를 진행하고 싶을 때의 용도로만 사용됩니다.
		TEST_METHOD(SnippetLeaves)
		{
			application::Application& client = application::Application::CreateApplication(0, 0);
			application::contents::ContentsLayer::AssignTestInitializer(SnippetInitializerLeaves);
			client.Initialize();
			client.Run();
			client.Finalize();
		}
	};
}
#endif
