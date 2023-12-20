#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void NVTestInitializerTemplate()
{
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

    auto debugStaticMesh = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    debugStaticMesh->GetGI().SetMesh(yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetMesh(L"Cube"));
    debugStaticMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color::blue());

    auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
    directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 100,10,0 }));

    auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::Camera>();

    yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace nonValidation
{
    TEST_CLASS(InWanderLand)
    {
    public:
        // nonValidation 네임스페이스에 속하는 테스트들은 빌드의 성공 여부를 판단하지 않습니다.
        // RunTests.bat를 실행해도 이 테스트들은 실행되지 않으며, Jenkins에서도 이 테스트들은 실행되지 않습니다.
        // 이 테스트들은 오직 개발자가 직접 엔트리 포인트를 달리하여 테스트를 진행하고 싶을 때의 용도로만 사용됩니다.
        TEST_METHOD(NonValTestTemplate)
        {
            Application::Application& client = Application::Application::CreateApplication(0, 0);
            Application::Contents::ContentsLayer::AssignTestInitializer(NVTestInitializerTemplate);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
}
#endif