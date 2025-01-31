#pragma once
#include "AutoAttackProjectile.h"

class EnemyAutoAttackProjectile : public AutoAttackProjectile
{
public:
	virtual void Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed, float offset) override;
	virtual void ReturnToPool() override;
};

