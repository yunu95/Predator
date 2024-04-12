#include "EnemyAutoAttackProjectile.h"

void EnemyAutoAttackProjectile::Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed, float offset)
{
	AutoAttackProjectile::Shoot(ownerUnit, opponentUnit, speed, offset);
}
