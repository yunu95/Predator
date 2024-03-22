#include "EnemyAutoAttackProjectile.h"

void EnemyAutoAttackProjectile::Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed)
{
	AutoAttackProjectile::Shoot(ownerUnit, opponentUnit, speed);
}
