#pragma once
#include "FieldDamage.h"

class StatusTimer;

/// <summary>
/// 우르술라 1번 스킬.
/// 도트 데미지 및 실명 상태이상 부여 스킬.
/// </summary>
class BlindFieldComponent : public FieldDamage
{
public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;

private:
	StatusTimer* m_blindTimer;

	float m_blindPersistTime;

	virtual void SetFieldSkillMembers() override;

};

