#pragma once
#include "SpecialEffectTrigger.h"
#include <list>
/// <summary>
/// 피격 유닛을 밀어내도록 하는 컴포넌트
/// </summary>

class StatusTimer;

class KnockBackComponent : public SpecialEffectTrigger
{
private:
	float m_pushPower = 6.5f;
	float m_duration = 0.5f;
	static constexpr float maxKnockHeight = 4;

	float m_ap = 1.0f;

	// 중복 충돌을 방지하기 위한 멤버
	std::list<Unit*> crushedUnitList;
	bool isSkillStarted;

public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;

public:
	virtual void Update() override;

public:
	void SetAP(float p_ap);
	void SkillStarted();
	void ClearCrushedUnitList();
};

