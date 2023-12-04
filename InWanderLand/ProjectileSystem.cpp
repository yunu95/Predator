#include "ProjectileSystem.h"
#include "BaseUnitEntity.h"
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
	for (int i = 0; i < 10; i++)
	{
		//m_projectileVector.push_back(yunutyEngine::Scene::getCurrentScene()->AddGameObject());
		
		m_projectileStack.push(yunutyEngine::Scene::getCurrentScene()->AddGameObject());
		m_projectileStack.top()->SetSelfActive(false);
		m_projectileStack.top()->AddComponent<Projectile>();
		m_projectileStack.top()->AddComponent<Dotween>();
		m_projectileStack.top()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		m_projectileStack.top()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().LoadMesh("Capsule");
		m_projectileStack.top()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 1, 0 });
	}
}

void ProjectileSystem::Shoot(BaseUnitEntity* ownerUnit, BaseUnitEntity* opponentUnit, float speed)
{
	if (m_projectileStack.empty())
	{
		SetUp();
	}

	m_projectileStack.top()->SetSelfActive(true);
	m_projectileStack.top()->GetComponent<Projectile>()->SetOwnerType(ownerUnit->GetType());
	m_projectileStack.top()->GetComponent<Projectile>()->Shoot(ownerUnit, opponentUnit, speed);
	m_projectileStack.pop();
}

void ProjectileSystem::ReturnToPool(GameObject* usedObject)
{
	m_projectileStack.push(usedObject);
}
