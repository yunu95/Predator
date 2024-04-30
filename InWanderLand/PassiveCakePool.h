#pragma once
#include "PassiveCake.h"
#include "DebugMeshes.h"
#include "UnitProductor.h"
#include "ContentsObservee.h"

class PassiveCakePool :
	public GameObjectPool<PassiveCake>, public Component, public SingletonComponent<PassiveCakePool>, public ContentsObservee
{
public:
	virtual void ObjectInitializer(PassiveCake* passiveCake) override
	{
		auto dotween = passiveCake->GetGameObject()->AddComponent<Dotween>();
		passiveCake->SetDotweenComponent(dotween);

		auto cakeMeshObject = Scene::getCurrentScene()->AddGameObject();
		auto cakeMesh = AttachDebugMesh(cakeMeshObject, DebugMeshType::Cube);
		cakeMesh->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::white(), false));
		cakeMesh->SetActive(false);
		passiveCake->SetMesh(cakeMesh);
		cakeMeshObject->GetTransform()->SetWorldScale({ UNIT_LENGTH, UNIT_LENGTH, UNIT_LENGTH });

		auto cakeCollider = passiveCake->GetGameObject()->AddComponent<physics::BoxCollider>();
		cakeCollider->SetHalfExtent({ UNIT_LENGTH * 0.5f, UNIT_LENGTH * 0.5f, UNIT_LENGTH * 0.5f });
		passiveCake->SetCollider(cakeCollider);

		passiveCake->GetGameObject()->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
		cakeCollider->SetActive(false);
	}

	virtual void Start() override;
	virtual void PlayFunction() override;
	virtual void StopFunction() override;
};

void PassiveCakePool::Start()
{
	isSingletonComponent = true;
}

void PassiveCakePool::PlayFunction()
{
	this->SetActive(true);
	if (isOncePaused)
	{
		Start();
	}
}

void PassiveCakePool::StopFunction()
{
	poolObjects.clear();
	expendableObjects.clear();
}
