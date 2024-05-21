#include "InWanderLand.h"
#include "RangedAttackSystem.h"
#include "AutoAttackProjectilePool.h"
#include "MagicianAutoAttackProjectilePool.h"
#include "HealerAutoAttackProjectilePool.h"
#include "EnemyAutoAttackProjectilePool.h"

void RangedAttackSystem::Attack(Unit* opponentUnit, float offset)
{
	m_currentReloadedProjectile->Shoot(m_ownerUnit, opponentUnit, m_bulletSpeed, offset);
}

void RangedAttackSystem::SetOwnerUnit(Unit* p_unit)
{
	m_ownerUnit = p_unit;
}

void RangedAttackSystem::SetBulletSpeed(float speed)
{
	m_bulletSpeed = speed;
}

void RangedAttackSystem::SetRange(float p_rng)
{
	m_range = p_rng;
}

void RangedAttackSystem::Start()
{
	m_ownerUnit = GetGameObject()->GetComponent<Unit>();
}

void RangedAttackSystem::ReloadProjectile()
{
	switch (m_ownerUnit->GetUnitType())
	{
	case Unit::UnitType::Magician:
	{
		m_currentReloadedProjectile = MagicianAutoAttackProjectilePool::Instance().Borrow();
		break;
	}
	case Unit::UnitType::Healer:
	{
		m_currentReloadedProjectile = HealerAutoAttackProjectilePool::Instance().Borrow();
		break;
	}
	case Unit::UnitType::RangedEnemy:
	{
		m_currentReloadedProjectile = HealerAutoAttackProjectilePool::Instance().Borrow();
		m_currentReloadedProjectile->SetStraightBulletRange(m_range);
		break;
	}
	default:
	{
		//AutoAttackProjectilePool::Instance().Borrow()->Shoot(m_ownerUnit, opponentUnit, m_bulletSpeed, offset);
		break;
	}
	}
}