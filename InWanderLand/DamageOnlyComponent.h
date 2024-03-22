#pragma once
#include "SpecialEffectTrigger.h"

class DamageOnlyComponent : public SpecialEffectTrigger
{
protected:
	float m_skillDamage;

public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;

public:
	virtual void SetSkillDamage(float dmg);
};
