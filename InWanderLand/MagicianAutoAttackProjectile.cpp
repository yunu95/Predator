#include "MagicianAutoAttackProjectile.h"
#include "MagicianAutoAttackProjectilePool.h"

void MagicianAutoAttackProjectile::Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed, float offset)
{
	AutoAttackProjectile::Shoot(ownerUnit, opponentUnit, speed, offset);
	
	/// 음향 버프 적용 : 공격속도와 공격력 중가
	//ownerUnit->IncreaseAttackPower(m_magicianAttackPowerIncrease);
	//ownerUnit->IncreaseAttackSpeed(m_magicianAttackPowerSpeedIncrease);
}

void MagicianAutoAttackProjectile::ReturnToPool()
{
	MagicianAutoAttackProjectilePool::Instance().Return(this);
}
