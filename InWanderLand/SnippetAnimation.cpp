#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void SnippetInitializerAnimation()
{
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

    auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

    rsrcManager->LoadFile("FBX/Boss");
    auto skinnedMesh = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("Boss");
    auto animator = skinnedMesh->GetComponent<yunutyEngine::graphics::Animator>();

    auto& animList = rsrcManager->GetAnimationList();
    yunuGI::IAnimation* anim = nullptr;
    for (auto each : animList)
    {
        if (each->GetName() == L"root|000.Idle")
        {
            anim = each;
        }
    }
    anim->SetLoop(true);
    animator->GetGI().PushAnimation(anim);
    animator->GetGI().Play(anim);

    auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
    directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 100,10,0 }));

    auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::Camera>();

    camObj->GetTransform()->SetWorldPosition({ 0,5,-15 });

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
        TEST_METHOD(SnippetAnimation)
        {
            Application::Application& client = Application::Application::CreateApplication(0, 0);
            Application::Contents::ContentsLayer::AssignTestInitializer(SnippetInitializerAnimation);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
}
#endif