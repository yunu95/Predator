#pragma once
#include "UnitStatusComponent.h"

class DamageOnlyComponent : public UnitStatusComponent
{
protected:
	float m_skillDamage;

public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;

public:
	virtual void SetSkillDamage(float dmg);
};
