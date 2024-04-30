#pragma once
#include "StatusEffectTrigger.h"

class DamageOnlyComponent : public StatusEffectTrigger
{
protected:
	float m_skillDamage;

public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;

public:
	virtual void SetSkillDamage(float dmg);
};
