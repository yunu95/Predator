#pragma once
#include "PassiveCake.h"
#include "DebugMeshes.h"
#include "ContentsLayer.h"
#include "Application.h"

class PassiveCakePool :
	public GameObjectPool<PassiveCake>, public SingletonClass<PassiveCakePool>
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

		auto cakeCollider = passiveCake->GetGameObject()->AddComponent<physics::BoxCollider>();
		passiveCake->SetCollider(cakeCollider);

		passiveCake->GetGameObject()->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
		cakeCollider->SetActive(false);

		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectVector(passiveCake->GetGameObject());
	}
};