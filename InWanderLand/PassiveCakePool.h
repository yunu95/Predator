#pragma once
#include "PassiveCake.h"
#include "DebugMeshes.h"

class PassiveCakePool :
	public GameObjectPool<PassiveCake>, public SingletonClass<PassiveCakePool>
{
public:
	virtual void ObjectInitializer(PassiveCake* passiveCake) override
	{
		passiveCake->GetGameObject()->AddComponent<Dotween>();

		auto cakeMesh = AttachDebugMesh(passiveCake->GetGameObject(), DebugMeshType::Cube);
		cakeMesh->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::white(), false));
		passiveCake->SetMesh(cakeMesh);

		auto cakeCollider = passiveCake->GetGameObject()->AddComponent<physics::BoxCollider>();
		passiveCake->SetCollider(cakeCollider);
		passiveCake->GetGameObject()->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	}
};