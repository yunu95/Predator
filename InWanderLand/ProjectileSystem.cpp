#include "ProjectileSystem.h"
#include "Unit.h"
#include "Projectile.h"

// 생성자에서 주인 오브젝트 등록.
ProjectileSystem::ProjectileSystem(Unit* OwnerUnit)
{
	m_ownerUnit = OwnerUnit;
}

void ProjectileSystem::SetProjectile(Projectile* p_projectile)
{
	p_projectile->SetSystem(this);
	p_projectile->GetGameObject()->SetSelfActive(false);
	projectileStack.push(p_projectile);
}

void ProjectileSystem::Shoot(Vector3d enemyPosition)
{
	Projectile* currentProjectile = projectileStack.top();
	projectileStack.pop();
	currentProjectile->GetGameObject()->GetTransform()->SetWorldPosition(m_ownerUnit->GetGameObject()->GetTransform()->GetWorldPosition());
	currentProjectile->GetGameObject()->SetSelfActive(true);
	currentProjectile->Shoot(enemyPosition);
}

void ProjectileSystem::ReturnToStack(Projectile* usedProjectile)
{
	projectileStack.push(usedProjectile);
	usedProjectile->GetGameObject()->SetSelfActive(false);
}

string ProjectileSystem::GetUnitType() const
{
	return m_ownerUnit->GetType();
}

bool ProjectileSystem::Empty() const
{
	return projectileStack.empty();
}
