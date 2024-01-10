#pragma once
#include "UnitStatusComponent.h"

/// <summary>
/// 오직 데미지만 주는 콜라이더에 붙는 컴포넌트.
/// 시간의 개념이 없어 피격 유닛의 Damage() 함수만 1회 호출해주면 된다.
/// </summary>
class OnlyDamageComponent : public UnitStatusComponent
{
private:
	int skillDamage;

public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;

public:
	void SetSkillDamage(int dmg);
};

