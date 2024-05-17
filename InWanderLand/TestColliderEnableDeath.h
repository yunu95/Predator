#pragma once
#include "RTSCam.h"
#include "DelayedTestFunctions.h"
#include "YunutyEngine.h"
#include "ShakyCam.h"
#include "DebugTilePlane.h"
#include "DebugBeacon.h"
#include "DebugMeshes.h"
#include "YunutyEngine.h"

class TriggerVolumeTest : virtual public Component
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
// PhysX Visual Debugger라는 프로그램을 실행하고 테스트 코드를 실행하면 어떤 현상이 일어나는지 더욱 자세히 관찰할 수 있습니다.
void TestColliderEnableDeath()
{
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
    //delayedFunctions->todoList.push_back({ 2,[=]() {Scene::getCurrentScene()->DestroyGameObject(pickingCollider); } });
    // 제대로 비활성화되는지 테스트
    delayedFunctions->todoList.push_back({ 1,[=]() {boxCollider->SetSelfActive(false); } });
    // 제대로 재활성화되는지 테스트
    delayedFunctions->todoList.push_back({ 3,[=]() {boxCollider->SetSelfActive(true); } });
}
