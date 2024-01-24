#pragma once
#include "AutoAttackProjectile.h"

class HealerAutoAttackProjectile : public AutoAttackProjectile
{
private:
	float currentPassiveCount = 0;
	float maxPassiveCount = 3;

public:
	virtual void Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed) override;
};

