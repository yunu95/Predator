#include "Interactable_BarricadeSphere.h"
#include "Projectile.h"
#include "DebugMeshes.h"
#include "InteractableData.h"

void Interactable_BarricadeSphere::SetDataFromEditorData(const application::editor::InteractableData& data)
{
	initPos.x = data.pod.position.x;
	initPos.y = data.pod.position.y;
	initPos.z = data.pod.position.z;
	initRotation.w = data.pod.rotation.w;
	initRotation.x = data.pod.rotation.x;
	initRotation.y = data.pod.rotation.y;
	initRotation.z = data.pod.rotation.z;
	initScale.x = data.pod.scale.x;
	initScale.y = data.pod.scale.y;
	initScale.z = data.pod.scale.z;
}

void Interactable_BarricadeSphere::Start()
{
	AttachDebugMesh(GetGameObject(), DebugMeshType::Sphere, yunuGI::Color::green());
	GetGameObject()->AddComponent<physics::SphereCollider>()->SetRadius(0.5f);
	GetGameObject()->AddComponent<physics::RigidBody>()->SetAsKinematic(true);

	auto ts = GetGameObject()->GetTransform();
	ts->SetWorldPosition(initPos);
	ts->SetWorldRotation(initRotation);
	ts->SetWorldScale(initScale);
}

void Interactable_BarricadeSphere::OnTriggerEnter(physics::Collider* collider)
{
	if (auto projectile = collider->GetGameObject()->GetComponent<Projectile>(); projectile != nullptr)
	{
		projectile->ExplodeAtCurrentPosition();
	}
}
