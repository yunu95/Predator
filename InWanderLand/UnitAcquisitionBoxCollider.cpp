#include "UnitAcquisitionBoxCollider.h"
#include "InWanderLand.h"

UnitAcquisitionBoxCollider::UnitAcquisitionBoxCollider()
    : physics::BoxCollider()
{
    debugMesh = GetGameObject()->AddGameObject();
    AttachDebugMesh(debugMesh, DebugMeshType::Cube, yunuGI::Color::green());
}

void UnitAcquisitionBoxCollider::SetHalfExtent(Vector3f halfExtent)
{
    physics::BoxCollider::SetHalfExtent(halfExtent);
    debugMesh->GetTransform()->SetWorldScale(2 * halfExtent);
}

void UnitAcquisitionBoxCollider::SetColor(yunuGI::Color color)
{
    auto dm = debugMesh->GetComponent<DebugStaticMesh>();
    dm->GetGI().GetMaterial()->SetColor(color);
}

void UnitAcquisitionBoxCollider::Update()
{
    physics::BoxCollider::Update();
    UnitAcquisitionCollider::Update();
}