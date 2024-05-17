#include "InWanderLand.h"
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
        GetGameObject()->GetTransform()->SetLocalPosition( { 3 * sin(t * 2), y, 0 });
    }
    /// <summary>
    /// OnTriggerEnter, OnTriggerExit 함수는 트리거 콜라이더와 다른 리지드바디 콜라이더가 서로 겹치거나, 충돌상태에서 벗어나는 시점에 호출됩니다.
    /// </summary>
    /// <param name="collider"></param>
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
    /// TriggerCollider는 물리 시뮬레이션에 영향을 받지 않으며, 다른 콜라이더와 겹치는지 여부만 알 수 있습니다.
    /// 게임 오브젝트에 Collider 컴포넌트만 추가하면 트리거 콜라이더가 됩니다.
    virtual void Start() override
    {
        TestDebugCollider::Start();
        collider = GetGameObject()->AddComponent<physics::BoxCollider>();
        collider->SetHalfExtent({ 0.5,0.5,0.5 });
		GetGameObject()->AddComponent<physics::RigidBody>();
    }
};
class KinematicCollider : public TestDebugCollider
{
    physics::BoxCollider* collider;
    physics::RigidBody* rigidBody;
    /// 키네마틱 콜라이더는 자체적으로 물리 시뮬레이션의 영향을 받지 않으나, 다른 일반 리지드바디 콜라이더들을 밀어냅니다.
    /// 키네마틱 콜라이더는 트리거 콜라이더와 겹칠 때 OnTrigger 이벤트가 호출됩니다.
    /// 게임 오브젝트에 Collider 컴포넌트와 rigidBody 컴포넌트를 추가하고, 리지드바디 객체의 키네마틱 플래그를 SetAsKinematic(true) 함수를 부르면 키네마틱 콜라이더가 됩니다.
    virtual void Start() override
    {
        TestDebugCollider::Start();
        collider = GetGameObject()->AddComponent<physics::BoxCollider>();
        collider->SetHalfExtent({ 0.5,0.5,0.5 });
        rigidBody = GetGameObject()->AddComponent<physics::RigidBody>();
        rigidBody->SetAsKinematic(true);
    }
};
void SnippetInitializerPhysxCollider()
{
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
    auto cam = Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::Camera>();
    cam->GetTransform()->SetLocalPosition( { 0,0,-10 });
    auto collider1 = Scene::getCurrentScene()->AddGameObject()->AddComponent<TriggerCollider>();
    auto collider2 = Scene::getCurrentScene()->AddGameObject()->AddComponent<KinematicCollider>();
    collider1->GetTransform()->SetLocalScale( { 1.5,1.5,1.5 });
    collider1->y = 0.3;
    collider2->y = -0.3;
    collider2->t = 1;

    auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
    directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 10,10,0 }));
}

namespace snippets
{
    TEST_CLASS(SnippetPhysxColliderClass)
    {
    public:
        /// <summary>
        /// SnippetPhysxCollider 코드는 PhysX 기능을 이용하여 3차원 공간에서 트리거 콜라이더 - 키네마틱 콜라이더간 상호작용을 테스트합니다.
        /// 두 박스 객체들은 좌우로 오가면서 서로 겹치면 색상이 빨간색으로 바뀌며, 겹치지 않으면 초록색으로 바뀝니다.
        /// </summary>
        TEST_METHOD(SnippetPhysxCollider)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::contents::ContentsLayer::AssignTestInitializer(SnippetInitializerPhysxCollider);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
}
#endif
