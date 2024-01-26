#include "HealerAutoAttackProjectile.h"
#include "PassiveCakePool.h"

void HealerAutoAttackProjectile::Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed)
{
	AutoAttackProjectile::Shoot(ownerUnit, opponentUnit, speed);

	/// 버프 중첩 마다 과자 생성
	
	HealerAutoAttackCount::currentPassiveCount++;

	if (HealerAutoAttackCount::currentPassiveCount >= maxPassiveCount)
	{
		HealerAutoAttackCount::currentPassiveCount = 0;
		PassiveCakePool::SingleInstance().Borrow()->PopCake(ownerUnit->GetTransform()->GetWorldPosition());
	}

}

void HealerAutoAttackProjectile::PlusHealerPassiveStack()
{
	HealerAutoAttackCount::currentPassiveCount++;
}
