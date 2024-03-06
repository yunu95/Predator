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
		//directionalLight->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{90,0,45} });
	}

	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	camObj->AddComponent<tests::GraphicsTestCam>();
	camObj->GetTransform()->SetLocalPosition(Vector3d{ 0,4,-15 });

	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();


	_resourceManager->LoadFile("CastleWallPS.cso");

	yunuGI::IShader* shader;
	auto& shaderList = _resourceManager->GetShaderList();
	for (auto& i : shaderList)
	{
		if (i->GetName() == L"CastleWallPS.cso")
		{
			shader = i;
		}
	}

	_resourceManager->LoadFile("FBX/SM_CastleWall");
	_resourceManager->LoadFile("Texture/T_Brick_Dirt_BaseColor.png");
	_resourceManager->LoadFile("Texture/T_Brick_Dirt_Normal.png");
	yunuGI::ITexture* texture;
	yunuGI::ITexture* texture2;
	auto& textureList = _resourceManager->GetTextureList();
	for (auto& i : textureList)
	{
		if (i->GetName() == L"Texture/T_Brick_Dirt_BaseColor.png")
		{
			texture = i;
		}

		if (i->GetName() == L"Texture/T_Brick_Dirt_Normal.png")
		{
			texture2 = i;
		}
	}
	{
		auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_CastleWall");
		auto& childVec = obj->GetChildren();
		childVec[0]->GetComponent<graphics::StaticMeshRenderer>()->GetGI().GetMaterial(1)->SetPixelShader(shader);
		childVec[0]->GetComponent<graphics::StaticMeshRenderer>()->GetGI().GetMaterial(1)->SetTexture(yunuGI::Texture_Type::Temp0, texture);
		childVec[0]->GetComponent<graphics::StaticMeshRenderer>()->GetGI().GetMaterial(1)->SetTexture(yunuGI::Texture_Type::Temp1, texture2);
		obj->GetTransform()->SetLocalPosition(Vector3d{ 0,0,10 });
	}

}

namespace snippets
{

	TEST_CLASS(SnippetCastleWallTextureBlendingClass)
	{
	public:
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
