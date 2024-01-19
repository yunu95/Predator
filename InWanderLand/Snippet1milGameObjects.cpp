#include "InWanderLand.h"
#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "DebugMeshes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void SnippetInitializer1milGameObjects()
{
    class UpdatingComponent : public yunutyEngine::Component
    {
        virtual void Update()override
        {
            int a = 0;
        }
    };
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
    for (auto i = 0; i < 1000000; i++)
    {
        auto gameObj = Scene::getCurrentScene()->AddGameObject();
        for (auto each : gameObj->GetComponents<Component>())
            each->SetIsUpdating(false);
    }
    for (auto i = 0; i < 100000; i++)
        Scene::getCurrentScene()->AddGameObject()->AddComponent<UpdatingComponent>();

    auto delayedTestFunctions = Scene::getCurrentScene()->AddGameObject()->AddComponent<DelayedTestFunctions>();
    delayedTestFunctions->todoList.push_back({ 2,[]() {
        int fps = Time::GetFPS();
    } });

    auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
    directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 100,10,0 }));

    auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::Camera>();
    camObj->GetTransform()->SetWorldPosition({ 0,0,-5 });

    yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace snippets
{
    TEST_CLASS(Snippet1milGameObjectsClass)
    {
    public:
        // 게임오브젝트의 갯수가 100만개가 되어도, 1000만개가 되어도 Update함수를 매번 호출하지만 않는다면 게임 실행에 전혀 부담을 주지 않습니다.
        // 이 코드에서는 게임 오브젝트를 100만개 생성해 게임 오브젝트 내의 컴포넌트들에게 SetIsUpdating(false)를 호출하여 Update함수를 호출하지 않도록 하였습니다.
        // 업데이트 함수가 정의된 컴포넌트들은 약 10만개까지 프레임 드랍없이 감당가능합니다.
        TEST_METHOD(Snippet1milGameObjects)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::Contents::ContentsLayer::AssignTestInitializer(SnippetInitializer1milGameObjects);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
};
#endif
