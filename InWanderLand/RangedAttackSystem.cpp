#include "RangedAttackSystem.h"
#include "ProjectileSystem.h"

void RangedAttackSystem::Attack(Unit* opponentUnit)
{
	ProjectileSystem::GetInstance()->Shoot(GetGameObject()->GetComponent<Unit>(),opponentUnit, m_bulletSpeed);
}

void RangedAttackSystem::SetOwnerUnit(Unit* p_unit)
{
	m_ownerUnit = p_unit;
}

void RangedAttackSystem::SetBulletSpeed(float speed)
{
	m_bulletSpeed = speed;
}
