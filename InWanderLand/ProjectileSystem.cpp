#include "InWanderLand.h"
#include "ProjectileSystem.h"
#include "Unit.h"
#include "Projectile.h"

ProjectileSystem* ProjectileSystem::instance = nullptr;

ProjectileSystem::ProjectileSystem()
{
	instance = this;
}

ProjectileSystem::~ProjectileSystem()
{
	delete instance;
}

ProjectileSystem* ProjectileSystem::GetInstance()
{
	if (instance == nullptr)
		instance = new ProjectileSystem;
	return instance;
}

void ProjectileSystem::SetUp()
{
	// 리소스 매니저 가져오기
	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	auto capsuleMesh = _resourceManager->GetMesh(L"Capsule");

	for (int i = 0; i < 10; i++)
	{
		//m_projectileVector.push_back(yunutyEngine::Scene::getCurrentScene()->AddGameObject());
		
		m_projectileStack.push(yunutyEngine::Scene::getCurrentScene()->AddGameObject());
		m_projectileStack.top()->SetSelfActive(false);
		m_projectileStack.top()->AddComponent<Projectile>();
		m_projectileStack.top()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		m_projectileStack.top()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().SetMesh(capsuleMesh);
		m_projectileStack.top()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 1, 0 });
	}
}

void ProjectileSystem::Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed)
{
	if (m_projectileStack.empty())
	{
		SetUp();
	}

	m_projectileStack.top()->SetSelfActive(true);
	m_projectileStack.top()->GetComponent<Projectile>()->SetOwnerType(ownerUnit->GetUnitType());
	m_projectileStack.top()->GetComponent<Projectile>()->Shoot(ownerUnit, opponentUnit, speed);
	m_projectileStack.pop();
}

void ProjectileSystem::ReturnToPool(GameObject* usedObject)
{
	m_projectileStack.push(usedObject);
}
