#pragma once
#include "YunutyEngine.h"

/// <summary>
/// 원하는 때에 자식으로부터 재정의된 ApplyStatus 함수를 호출하여 사용한다.
/// </summary>

class Unit;

class SpecialEffect : public Component
{
protected:
	Unit* m_ownerUnit;

	friend class DualCastComponent;

public:
	virtual void Start() override;
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) = 0;
	virtual void SetSkillOwnerUnit(Unit* p_unit);
};

