#include "InWanderLand.h"
#include "RangedAttackSystem.h"
#include "AutoAttackProjectilePool.h"
#include "MagicianAutoAttackProjectilePool.h"
#include "HealerAutoAttackProjectilePool.h"
#include "EnemyAutoAttackProjectilePool.h"

void RangedAttackSystem::Attack(Unit* opponentUnit)
{
	switch (m_ownerUnit->GetUnitType())
	{
		case Unit::UnitType::Magician :
		{
			MagicianAutoAttackProjectilePool::SingleInstance().Borrow()->Shoot(m_ownerUnit, opponentUnit, m_bulletSpeed);
			break;
		}
		case Unit::UnitType::Healer:
		{
			auto tempHealerBullet = HealerAutoAttackProjectilePool::SingleInstance().Borrow();
			tempHealerBullet->GetGameObject()->SetSelfActive(false);

			tempHealerBullet->Shoot(m_ownerUnit, opponentUnit, m_bulletSpeed);
			break;
		}
		case Unit::UnitType::RangedEnemy:
		{
			EnemyAutoAttackProjectilePool::SingleInstance().Borrow()->Shoot(m_ownerUnit, opponentUnit, m_bulletSpeed);
			break;
		}
		default:
		{
			AutoAttackProjectilePool::SingleInstance().Borrow()->Shoot(m_ownerUnit, opponentUnit, m_bulletSpeed);
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

void RangedAttackSystem::Start()
{
	m_ownerUnit = GetGameObject()->GetComponent<Unit>();
}
