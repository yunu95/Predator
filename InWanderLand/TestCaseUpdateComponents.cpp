#include "InWanderLand.h"
#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
    TEST_CLASS(TestCaseUpdateComponentsClass)
    {
        class UpdateComponent : public yunutyEngine::Component
        {
        public:
            static int mustNotBeZero;
            static bool testFailedForSure;
            int count = 0;
            bool checkMustNotBeZero{ true };
            void Update() override
            {
                if (checkMustNotBeZero && (mustNotBeZero == 0))
                    testFailedForSure = true;
                ++mustNotBeZero;
                ++count;
            }
        };
        static void TestInitializerUpdateComponents()
        {
            yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
            auto grandDaddy = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
            auto daddy = grandDaddy->AddGameObject();
            auto child = daddy->AddGameObject();
            auto child2 = daddy->AddGameObject();
            auto daddyComp = daddy->AddComponent<UpdateComponent>();
            auto comp = child->AddComponent<UpdateComponent>();
            auto comp2 = child2->AddComponent<UpdateComponent>();

            for (auto i = 0; i < 100; ++i)
                daddy->AddGameObject()->AddComponent<UpdateComponent>();

            daddyComp->checkMustNotBeZero = false;
            comp2->SetIsUpdating(false);
            auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
            directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 100,10,0 }));
            auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::graphics::Camera>();

            auto delayedTestFunctions = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DelayedTestFunctions>();

            // delayedTestFunctions에 2초 후 실행시킬 콜백 함수를 등록합니다. 이 콜백함수는 게임 엔진 스레드에서 호출됩니다.
            delayedTestFunctions->todoList.push_back({ 0.3,[=]() {
                // 게임 엔진 스레드에서 메인 스레드에서 특정 동작을 구동시키고 싶다면 아래의 AddMainLoopTodo 함수를 사용합니다.
                application::Application::GetInstance().AddMainLoopTodo([=]() {
                    // Assert 함수군은 테스트 케이스의 실행 성공 여부를 판단하는데에 쓰입니다.
                    // Assert의 실행은 메인 스레드에서 실행되어야 합니다.
                    Assert::IsTrue(comp->count != 0);
                    Assert::IsTrue(comp2->count == 0);
                    Assert::IsTrue(UpdateComponent::testFailedForSure == false);

                    // 위 식이 참이라면 프로그램을 종료합니다. 
                    application::Application::GetInstance().TurnOff();
                        });
                    } });


            yunutyEngine::YunutyCycle::SingleInstance().Play();
        }
    public:
        // 부모 게임오브젝트에 달린 컴포넌트의 Update가 먼저 호출되며,
        // 자식 게임오브젝트에 달린 컴포넌트의 Update가 다음으로 호출되는지 테스트합니다.
        // 또한 컴포넌트의 SetIsUpdating 함수가 제대로 작동하는지 테스트합니다.
        TEST_METHOD(TestCaseUpdateComponents)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::contents::ContentsLayer::AssignTestInitializer(TestInitializerUpdateComponents);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
}

bool tests::TestCaseUpdateComponentsClass::UpdateComponent::testFailedForSure{false};
int tests::TestCaseUpdateComponentsClass::UpdateComponent::mustNotBeZero = 0;
#endif