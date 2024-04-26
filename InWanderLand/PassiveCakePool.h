#pragma once
#include "PassiveCake.h"
#include "DebugMeshes.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "LazySingletonClass.h"
#include "UnitProductor.h"

class PassiveCakePool :
	public GameObjectPool<PassiveCake>, public GHContents::LazySingletonClass<PassiveCakePool>
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

		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectContainer(passiveCake->GetGameObject());
		contentsLayer->RegisterToEditorObjectContainer(cakeMeshObject);
	}
};