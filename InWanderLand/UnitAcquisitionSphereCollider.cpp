#include "UnitAcquisitionSphereCollider.h"
#include "InWanderLand.h"

UnitAcquisitionSphereCollider::UnitAcquisitionSphereCollider()
    : SphereCollider()
{
    debugMesh = GetGameObject()->AddGameObject();
    AttachDebugMesh(debugMesh, DebugMeshType::Sphere, yunuGI::Color::green());
}

void UnitAcquisitionSphereCollider::SetRadius(float radius)
{
    physics::SphereCollider::SetRadius(radius);
    debugMesh->GetTransform()->SetWorldScale(2 * radius * Vector3d::one);
}

void UnitAcquisitionSphereCollider::SetColor(yunuGI::Color color)
{
    auto dm = debugMesh->GetComponent<DebugStaticMesh>();
    dm->GetGI().GetMaterial()->SetColor(color);
}

void UnitAcquisitionSphereCollider::Update()
{
    physics::SphereCollider::Update();
    UnitAcquisitionCollider::Update();
}
