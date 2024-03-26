#include "InWanderLand.h"
#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "TestUtilGraphicsTestCam.h"
#include "SpriteAnimation.h"
#include "WanderUtils.h"
#include "Billboard.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void SnippetInitializerBillboardSpriteAnim()
{
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
    auto rsrc = graphics::Renderer::SingleInstance().GetResourceManager();
    wanderUtils::LoadResourcesRecursively();
    auto gameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto gameObject2 = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto staticMesh2 = gameObject2->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    staticMesh2->GetGI().SetMesh(rsrc->GetMesh(L"Cube"));
    gameObject2->GetTransform()->SetLocalPosition({ 0,4,0 });
    auto anim = gameObject->AddComponent<SpriteAnimation>();

    anim->SetSprites(L"Texture/fire_forward");
    auto billboard = gameObject->AddComponent<yunutyEngine::Billboard>();
    auto billboard2 = gameObject2->AddComponent<yunutyEngine::Billboard>();
    auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
    directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 100,10,0 }));

    auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<tests::GraphicsTestCam>();

    camObj->GetTransform()->SetWorldPosition({ 0,0,-15 });
}

namespace snippets
{
    TEST_CLASS(SnippetBillboardSpriteAnimClass)
    {
    public:
        // 테스트 함수의 이름이 Snippet으로 시작하는 테스트들은 빌드의 성공 여부 판단에 쓰이지 않습니다.
        // RunTests.bat를 실행해도 이 테스트들은 실행되지 않으며, Jenkins에서도 이 테스트들은 실행되지 않습니다.
        // 이 테스트들은 오직 개발자가 직접 엔트리 포인트를 달리하여 테스트를 진행하고 싶을 때의 용도로만 사용됩니다.
        TEST_METHOD(SnippetBillboardSpriteAnim)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::contents::ContentsLayer::AssignTestInitializer(SnippetInitializerBillboardSpriteAnim);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
}
#endif