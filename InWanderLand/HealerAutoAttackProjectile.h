#pragma once
#include "AutoAttackProjectile.h"

namespace HealerAutoAttackCount
{
	static int currentPassiveCount = 0;
}

class HealerAutoAttackProjectile : public AutoAttackProjectile
{
private:
	int maxPassiveCount = 5;

public:
	virtual void Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed, float offset) override;
	virtual void ReturnToPool() override;

public:
	static void PlusHealerPassiveStack();
};

