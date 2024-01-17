#pragma once
#include "UnitStatusComponent.h"
/// <summary>
/// Collider가 존재하는 시간동안 주기적으로 tick 데미지를 주는 컴포넌트 
/// Bleeding과의 차이점은 단 하나 -> 장판을 밟고 있는가에 대한 판단
/// </summary>

class StatusTimer;

class FieldDamage : public UnitStatusComponent
{
private:
	struct UnitSensor
	{
		Unit* opponentUnit;
		bool isInField;
	};

	std::map<UnitSensor*, StatusTimer*> opponentUnitMap;

	Unit* unitOnField;

	float m_fieldDamage = 7.0f;
	float m_fieldDamageDelay = 0.2f;

public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;
	virtual void Start() override;

public:
	virtual void OnTriggerExit(physics::Collider* collider) override;
};

