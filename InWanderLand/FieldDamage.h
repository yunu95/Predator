#pragma once
#include "StatusEffectTrigger.h"
/// <summary>
/// 장판 스킬의 부모 클래스.
/// 기본적인 틱 데미지를 주는 기능을 제공하며, 추가적인 요소 (ex.실명 및 끌어당김)은
/// 자식 클래스에서 따로 멤버 선언을 해준다.
/// </summary>

class StatusTimer;

class FieldDamage : public StatusEffectTrigger
{
protected:
	StatusTimer* m_damageTimer;			// 기본적인 장판 틱 데미지 타이머, 추가로 사용하고자 할 땐 자식 클래스에서 다른 타이머를 새로 만들어주기.
	
	std::set<Unit*> m_currentOnFieldUnits;

	float m_fieldDamageElapsed = 0.0f;

	float m_fieldDamage;
	float m_fieldDamageDelay;

	virtual void SetFieldSkillMembers() = 0;

public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;
	virtual void Start() override;
	virtual void Update() override;

public:
	virtual void OnTriggerExit(physics::Collider* collider) override;

public:
	virtual void ActivateFieldTimer();
};

