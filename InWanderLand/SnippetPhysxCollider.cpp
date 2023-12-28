#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "DebugMeshes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

class TestDebugCollider : public yunutyEngine::Component
{
public:
    float t;
private:
    graphics::StaticMeshRenderer* debugMesh;
protected:
    virtual void Start() override
    {
        debugMesh = AttachDebugMesh(GetGameObject(), DebugMeshType::Cube, yunuGI::Color::green());
    }
    virtual void Update() override
    {
        t += Time::GetDeltaTime();
        GetGameObject()->GetTransform()->position = { 0,0,3 * sin(t) };
    }
    virtual void OnTriggerEnter(physics::Collider* collider) override
    {
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
// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void SnippetInitializerPhysxCollider()
{
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
    auto cam = Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::Camera>();
    cam->GetTransform()->position = { 0,0,-10 };
    //auto collider1 = Scene::getCurrentScene()->AddGameObject()->AddComponent<TriggerCollider>();
    //auto collider2 = Scene::getCurrentScene()->AddGameObject()->AddComponent<KinematicCollider>();
    //collider2->t = 1;

    auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
    directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 100,10,0 }));
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
        TEST_METHOD(SnippetPhysxCollider)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::Contents::ContentsLayer::AssignTestInitializer(SnippetInitializerPhysxCollider);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
}
#endif