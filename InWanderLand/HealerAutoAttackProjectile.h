#pragma once
#include "AutoAttackProjectile.h"

class HealerAutoAttackProjectile : public AutoAttackProjectile
{
private:
	int maxPassiveCount = 5;

public:
	virtual void Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed) override;
};

