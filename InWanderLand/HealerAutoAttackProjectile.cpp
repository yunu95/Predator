#include "HealerAutoAttackProjectile.h"
#include "PassiveCakePool.h"

void HealerAutoAttackProjectile::Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed)
{
	AutoAttackProjectile::Shoot(ownerUnit, opponentUnit, speed);

	/// 버프 중첩 마다 과자 생성
	//passiveCake->GetGameObject()->SetSelfActive(false);

	HealerAutoAttackCount::currentPassiveCount++;

	if (HealerAutoAttackCount::currentPassiveCount >= maxPassiveCount)
	{
		auto passiveCake = PassiveCakePool::SingleInstance().Borrow();
		HealerAutoAttackCount::currentPassiveCount = 0;
		passiveCake->PopCake(ownerUnit->GetTransform()->GetWorldPosition());
	}

}

void HealerAutoAttackProjectile::PlusHealerPassiveStack()
{
	HealerAutoAttackCount::currentPassiveCount++;
}
