#include "RangedAttackSystem.h"
#include "ProjectileSystem.h"

void RangedAttackSystem::Attack(Unit* opponentUnit)
{
	ProjectileSystem::GetInstance()->Shoot(m_ownerUnit, opponentUnit, m_bulletSpeed);
}

void RangedAttackSystem::SetOwnerUnit(Unit* p_unit)
{
	m_ownerUnit = p_unit;
}

void RangedAttackSystem::SetBulletSpeed(float speed)
{
	m_bulletSpeed = speed;
}

void RangedAttackSystem::Start()
{
	m_ownerUnit = GetGameObject()->GetComponent<Unit>();
}
