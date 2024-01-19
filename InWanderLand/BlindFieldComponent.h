#pragma once
#include "FieldDamage.h"

class StatusTimer;

class BlindFieldComponent : public FieldDamage
{
public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;
};

