#pragma once
#include "AutoAttackProjectile.h"

class MagicianAutoAttackProjectile : public AutoAttackProjectile
{
private:
	float m_magicianAttackPowerIncrease = 1.0f;
	float m_magicianAttackPowerSpeedIncrease = 1.2f;

public:
	virtual void Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed) override;
};

