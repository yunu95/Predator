#include "EnemyAutoAttackProjectile.h"
#include "EnemyAutoAttackProjectilePool.h"

void EnemyAutoAttackProjectile::Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed, float offset)
{
	AutoAttackProjectile::Shoot(ownerUnit, opponentUnit, speed, offset);
}

void EnemyAutoAttackProjectile::ReturnToPool()
{
	EnemyAutoAttackProjectilePool::Instance().Return(this);
}

