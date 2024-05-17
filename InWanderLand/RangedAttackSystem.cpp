#include "InWanderLand.h"
#include "RangedAttackSystem.h"
#include "AutoAttackProjectilePool.h"
#include "MagicianAutoAttackProjectilePool.h"
#include "HealerAutoAttackProjectilePool.h"
#include "EnemyAutoAttackProjectilePool.h"

void RangedAttackSystem::Attack(Unit* opponentUnit, float offset)
{
	switch (m_ownerUnit->GetUnitType())
	{
		case Unit::UnitType::Magician :
		{
			MagicianAutoAttackProjectilePool::Instance().Borrow()->Shoot(m_ownerUnit, opponentUnit, m_bulletSpeed, offset);
			break;
		}
		case Unit::UnitType::Healer:
		{
			HealerAutoAttackProjectilePool::Instance().Borrow()->Shoot(m_ownerUnit, opponentUnit, m_bulletSpeed, offset);
			break;
		}
		case Unit::UnitType::RangedEnemy:
		{
			EnemyAutoAttackProjectile* projectile = EnemyAutoAttackProjectilePool::Instance().Borrow();
			projectile->SetStraightBulletRange(m_range);
			projectile->Shoot(m_ownerUnit, opponentUnit, m_bulletSpeed, offset);
			break;
		}
		default:
		{
			//AutoAttackProjectilePool::Instance().Borrow()->Shoot(m_ownerUnit, opponentUnit, m_bulletSpeed, offset);
			break;
		}
	}
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
