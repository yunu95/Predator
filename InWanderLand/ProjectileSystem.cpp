#include "ProjectileSystem.h"
#include "Unit.h"
#include "Projectile.h"
#include "Dotween.h"

ProjectileSystem* ProjectileSystem::instance = nullptr;

ProjectileSystem::ProjectileSystem()
{
	instance = this;
}

ProjectileSystem* ProjectileSystem::GetInstance()
{
	if (instance == nullptr)
		instance = new ProjectileSystem;
	return instance;
}

void ProjectileSystem::SetUp()
{
	for (int i = 0; i < 100; i++)
	{
		m_projectileStack.push(yunutyEngine::Scene::getCurrentScene()->AddGameObject());
		m_projectileStack.top()->SetSelfActive(false);
		m_projectileStack.top()->AddComponent<Projectile>();
		m_projectileStack.top()->AddComponent<CircleCollider2D>();
		m_projectileStack.top()->AddComponent<Dotween>();
		m_projectileStack.top()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		m_projectileStack.top()->GetComponent<CircleCollider2D>()->SetRadius(0.01f);
		m_projectileStack.top()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().LoadMesh("Capsule");
		m_projectileStack.top()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 1, 0 });
	}
}

void ProjectileSystem::Shoot(Unit* ownerUnit, Vector3d endPosition, float speed)
{
	if (m_projectileStack.empty())
	{
		SetUp();
	}

	Vector3d startPosition = ownerUnit->GetGameObject()->GetTransform()->GetWorldPosition();
	float distance = (endPosition - startPosition).Magnitude();

	m_projectileStack.top()->SetSelfActive(true);
	m_projectileStack.top()->GetComponent<Projectile>()->SetOwnerType(ownerUnit->GetType());
	m_projectileStack.top()->GetTransform()->SetWorldPosition(startPosition);
	m_projectileStack.top()->GetComponent<Dotween>()->DOMove(endPosition, distance / speed);
	m_projectileStack.pop();
}

void ProjectileSystem::ReturnToPool(GameObject* usedObject)
{
	usedObject->SetSelfActive(false);

	m_projectileStack.push(usedObject);
}
