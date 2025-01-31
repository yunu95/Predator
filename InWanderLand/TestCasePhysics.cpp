#include "InWanderLand.h"
#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "RTSCam.h"
#include "DelayedTestFunctions.h"
#include "YunutyEngine.h"
#include "ShakyCam.h"
#include "DebugTilePlane.h"
#include "DebugBeacon.h"
#include "DebugMeshes.h"
#include "YunutyEngine.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

class TriggerVolumeTest : public Component
{
    // PhysX Visual Debugger라는 프로그램을 실행하고 테스트 코드를 실행하면 어떤 현상이 일어나는지 더욱 자세히 관찰할 수 있습니다.
    virtual void OnTriggerEnter(physics::Collider* other) override
    {
        DebugBeacon::PlaceBeacon(other->GetTransform()->GetWorldPosition(), yunuGI::Color::blue(), { 3,3,3 });
    };
    virtual void OnTriggerExit(physics::Collider* other) override
    {
        DebugBeacon::PlaceBeacon(other->GetTransform()->GetWorldPosition(), yunuGI::Color::blue(), { 3,3,3 });
    };
};

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void TestCasePhysicsInit()
{
    yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
    auto delayedTestFunctions = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DelayedTestFunctions>();

    auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto rtsCam = camObj->AddComponent<RTSCam>();
    rtsCam->GetTransform()->SetLocalPosition( Vector3d(0, 10, 0));

    auto boxCollider = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto boxColliderComp = boxCollider->AddComponent<yunutyEngine::physics::BoxCollider>();
    auto rigidComp = boxCollider->AddComponent<physics::RigidBody>();
    //rigidComp->SetAsStatic(true);
    boxCollider->GetTransform()->SetLocalPosition( Vector3d(0, 10, 0));
    boxCollider->GetTransform()->SetLocalRotation( Vector3d(100, 10, 35));
    auto boxMesh = boxCollider->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();

    auto resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    auto cubeMesh = resourceManager->GetMesh(L"Cube");
    auto sphereMeshResource = resourceManager->GetMesh(L"Sphere");
    auto capsuleMeshResource = resourceManager->GetMesh(L"Capsule");

    boxMesh->GetGI().SetMesh(cubeMesh);
    boxMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color::black());
    {
        auto boxCollider = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto boxColliderComp = boxCollider->AddComponent<yunutyEngine::physics::BoxCollider>();
        auto rigidComp = boxCollider->AddComponent<physics::RigidBody>();
        //rigidComp->SetAsStatic(true);
        boxCollider->GetTransform()->SetLocalPosition( Vector3d(0, 13, 0));
        boxCollider->GetTransform()->SetLocalRotation( Vector3d(100, 10, 35));
        auto boxMesh = boxCollider->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        boxCollider->AddComponent<TriggerVolumeTest>();
        boxMesh->GetGI().SetMesh(cubeMesh);
        boxMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color::black());
    }
    {
        auto sphereCollider = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto sphereColliderComp = sphereCollider->AddComponent<yunutyEngine::physics::SphereCollider>();
        auto rigidComp = sphereCollider->AddComponent<physics::RigidBody>();
        //rigidComp->SetAsStatic(true);
        sphereCollider->GetTransform()->SetLocalPosition( Vector3d(-1, 13, 2));
        sphereCollider->GetTransform()->SetLocalRotation( Vector3d(100, 10, 35));
        auto sphereMesh = sphereCollider->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        sphereCollider->AddComponent<TriggerVolumeTest>();
        sphereMesh->GetGI().SetMesh(sphereMeshResource);
        sphereMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color::black());
        sphereColliderComp->SetRadius(0.5);
    }
    {
        auto capsuleCollider = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto capsuleColliderComp = capsuleCollider->AddComponent<yunutyEngine::physics::CapsuleCollider>();
        auto rigidComp = capsuleCollider->AddComponent<physics::RigidBody>();
        //rigidComp->SetAsStatic(true);
        capsuleCollider->GetTransform()->SetLocalPosition( Vector3d(1, 13, 2));
        capsuleCollider->GetTransform()->SetLocalRotation( Vector3d(100, 10, 35));
        auto capsuleMesh = capsuleCollider->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
        capsuleCollider->AddComponent<TriggerVolumeTest>();
        capsuleMesh->GetGI().SetMesh(capsuleMeshResource);
        capsuleMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color::black());
        capsuleColliderComp->SetRadius(0.25);
        capsuleColliderComp->SetHalfHeight(0.5);
    }

    auto planeCollider = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto planeColliderComp = planeCollider->AddComponent<yunutyEngine::physics::BoxCollider>();
    planeColliderComp->SetHalfExtent({ 5,1,5 });
    auto planeRigidComp = planeCollider->AddComponent<physics::RigidBody>();
    planeRigidComp->SetAsStatic(true);
    planeCollider->GetTransform()->SetLocalPosition( Vector3d(0, 0, 0));
    auto planeMesh = planeCollider->AddGameObject();
    auto planeMeshComp = planeMesh->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
    planeMeshComp->GetGI().SetMesh(cubeMesh);
    planeMeshComp->GetGI().GetMaterial()->SetColor(yunuGI::Color::white());
    planeMesh->GetTransform()->SetLocalScale( Vector3d(10, 2, 10));

    // 트리거 콜라이더 생성
    /*{
        auto planeCollider = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto planeColliderComp = planeCollider->AddComponent<yunutyEngine::physics::BoxCollider>();
        planeColliderComp->SetHalfExtent({ 10,1,10 });
        planeCollider->GetTransform()->position = Vector3d(0, 3, 0);
    }*/
    auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::DirectionalLight>();
    directionalLight->GetTransform()->SetWorldRotation(Quaternion({ 100,10,0 }));

    auto delayedFunctions = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DelayedTestFunctions>();

    // 제대로 삭제되는지 테스트
    delayedFunctions->todoList.push_back({ 2,[=]() {Scene::getCurrentScene()->DestroyGameObject(boxCollider); } });
    // 제대로 비활성화되는지 테스트
    //delayedFunctions->todoList.push_back({ 1,[=]() {boxColliderComp->SetActive(false); } });
    // 제대로 재활성화되는지 테스트
    //delayedFunctions->todoList.push_back({ 1.3,[=]() {boxColliderComp->SetActive(true); } });

    // delayedTestFunctions에 2초 후 실행시킬 콜백 함수를 등록합니다. 이 콜백함수는 게임 엔진 스레드에서 호출됩니다.
    delayedTestFunctions->todoList.push_back({ 2,[=]() {
        // 게임 엔진 스레드에서 메인 스레드에서 특정 동작을 구동시키고 싶다면 아래의 AddMainLoopTodo 함수를 사용합니다.
        application::Application::GetInstance().AddMainLoopTodo([=]() {
            // Assert 함수군은 테스트 케이스의 실행 성공 여부를 판단하는데에 쓰입니다.
            // Assert의 실행은 메인 스레드에서 실행되어야 합니다.
            Assert::IsTrue(boxCollider->GetTransform()->GetWorldPosition().y < 3);

            // 위 식이 참이라면 프로그램을 종료합니다. 
            application::Application::GetInstance().TurnOff();
                });
            } });

}

namespace tests
{
    TEST_CLASS(TestCasePhysicsClass)
    {
    public:
        TEST_METHOD(TestCasePhysics)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::contents::ContentsLayer::AssignTestInitializer(TestCasePhysicsInit);
            client.Initialize();
            client.Run();
            client.Finalize();
        }
    };
}
#endif
