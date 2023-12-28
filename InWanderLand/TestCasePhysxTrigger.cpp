#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "DebugMeshes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void SnippetInitializerPhysxTrigger()
{
    class TestDebugCollider : public yunutyEngine::Component
    {
    public:
        float t;
        float y;
        int triggerCount{ 0 };
    private:
        graphics::StaticMeshRenderer* debugMesh;
    protected:
        virtual void Start() override
        {
            debugMesh = AttachDebugMesh(GetGameObject(), DebugMeshType::Cube, yunuGI::Color::green(), false);
        }
        virtual void Update() override
        {
            t += Time::GetDeltaTime();
            GetGameObject()->GetTransform()->position = { 3 * sin(t * 10), y, 0 };
        }
        virtual void OnTriggerEnter(physics::Collider* collider) override
        {
            triggerCount++;
            debugMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color::red());
        }
        virtual void OnTriggerExit(physics::Collider* collider) override
        {
            debugMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color::green());
        }
    };
    class TriggerCollider : public TestDebugCollider
    {
    public:
        physics::BoxCollider* collider;
        virtual void Start() override
        {
            TestDebugCollider::Start();
            collider = GetGameObject()->AddComponent<physics::BoxCollider>();
            collider->SetHalfExtent({ 0.5,0.5,0.5 });
        }
    };
    class KinematicCollider : public TestDebugCollider
    {
        physics::BoxCollider* collider;
        physics::RigidBody* rigidBody;
        virtual void Start() override
        {
            TestDebugCollider::Start();
            collider = GetGameObject()->AddComponent<physics::BoxCollider>();
            collider->SetHalfExtent({ 0.5,0.5,0.5 });
            rigidBody = GetGameObject()->AddComponent<physics::RigidBody>();
            rigidBody->SetAsKinematic(true);
        }
    };
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
    auto cam = Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::Camera>();
    cam->GetTransform()->position = { 0,0,-10 };
    auto collider1 = Scene::getCurrentScene()->AddGameObject()->AddComponent<TriggerCollider>();
    auto collider2 = Scene::getCurrentScene()->AddGameObject()->AddComponent<KinematicCollider>();
    collider1->y = 0.3;
    collider2->y = -0.3;
    collider2->t = 1;

    auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
    directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 10,10,0 }));
    yunutyEngine::YunutyCycle::SingleInstance().Play();
    DelayedTestFunctions* delayedTestFunctions = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DelayedTestFunctions>();
    delayedTestFunctions->todoList.push_back({ 2,[=]() {
        Assert::IsTrue(collider1->triggerCount == 6);
        application::Application::GetInstance().TurnOff();
    } });
}

namespace InWanderLand
{
    TEST_CLASS(InWanderLand)
    {
    public:
        TEST_METHOD(TestCasePhysxTrigger)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::Contents::ContentsLayer::AssignTestInitializer(SnippetInitializerPhysxTrigger);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
}
#endif