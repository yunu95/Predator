#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void TestInitializerTemplate()
{
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
    auto delayedTestFunctions = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DelayedTestFunctions>();

    // delayedTestFunctions에 2초 후 실행시킬 콜백 함수를 등록합니다. 이 콜백함수는 게임 엔진 스레드에서 호출됩니다.
    delayedTestFunctions->todoList.push_back({ 2,[]() {
        // 게임 엔진 스레드에서 메인 스레드에서 특정 동작을 구동시키고 싶다면 아래의 AddMainLoopTodo 함수를 사용합니다.
        Application::Application::GetInstance().AddMainLoopTodo([]() {
            // Assert 함수군은 테스트 케이스의 실행 성공 여부를 판단하는데에 쓰입니다.
            // Assert의 실행은 메인 스레드에서 실행되어야 합니다.
            Assert::IsTrue(1 + 1 == 3);

            // 위 식이 참이라면 프로그램을 종료합니다. 
            Application::Application::GetInstance().TurnOff();
                });
            } });

    auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::Camera>();

    yunutyEngine::YunutyCycle::SingleInstance().Play();
}

namespace InWanderLand
{
    TEST_CLASS(InWanderLand)
    {
    public:
        TEST_METHOD(TestCaseTemplate)
        {
            Application::Application& client = Application::Application::CreateApplication(0, 0);
            Application::Contents::ContentsLayer::AssignTestInitializer(TestInitializerTemplate);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
}