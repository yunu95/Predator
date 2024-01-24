#include "HealerAutoAttackProjectile.h"

void HealerAutoAttackProjectile::Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed)
{
	AutoAttackProjectile::Shoot(ownerUnit, opponentUnit, speed);

	/// 버프 중첩 마다 과자 생성
	
}
