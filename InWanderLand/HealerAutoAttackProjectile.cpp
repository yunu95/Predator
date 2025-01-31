#include "HealerAutoAttackProjectile.h"
#include "HealerAutoAttackProjectilePool.h"
#include "PassiveCakePool.h"

void HealerAutoAttackProjectile::Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed, float offset)
{
	AutoAttackProjectile::Shoot(ownerUnit, opponentUnit, speed, offset);

	/// 버프 중첩 마다 과자 생성
	//passiveCake->GetGameObject()->SetSelfActive(false);

	HealerAutoAttackCount::currentPassiveCount++;

	if (HealerAutoAttackCount::currentPassiveCount >= maxPassiveCount)
	{
		auto passiveCake = PassiveCakePool::Instance().Borrow();
		HealerAutoAttackCount::currentPassiveCount = 0;
		passiveCake->PopCake(ownerUnit->GetTransform()->GetWorldPosition());
	}

}

void HealerAutoAttackProjectile::ReturnToPool()
{
	HealerAutoAttackProjectilePool::Instance().Return(this);
}

void HealerAutoAttackProjectile::PlusHealerPassiveStack()
{
	HealerAutoAttackCount::currentPassiveCount++;
}
