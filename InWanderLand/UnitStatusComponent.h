#pragma once
#include "YunutyEngine.h"
/// <summary>
/// 출혈과 같은 상태이상과 버프 상태 컴포넌트.
/// </summary>

class Unit;

class UnitStatusComponent : public Component
{
public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) = 0;
};

