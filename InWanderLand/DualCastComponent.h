#pragma once
#include "SpecialEffectTrigger.h"

/// <summary>
/// 피격된 아군에게는 버프, 적군에게는 디버프를 부여하는 컴포넌트.
/// </summary>
class DualCastComponent : public SpecialEffectTrigger
{
public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;
	
public:
	virtual void OnTriggerEnter(physics::Collider* collider) override;
};

