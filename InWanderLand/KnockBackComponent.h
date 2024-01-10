#pragma once
#include "UnitStatusComponent.h"
/// <summary>
/// 피격 유닛을 밀어내도록 하는 컴포넌트
/// </summary>

class StatusTimer;

class KnockBackComponent : public UnitStatusComponent
{
private:
	float m_power = 3.0f;
	float m_duration = 0.2f;

public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;

public:
	virtual void Update() override;
};

