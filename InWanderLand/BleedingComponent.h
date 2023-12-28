#pragma once
#include "UnitStatusComponent.h"

class StatusTimer;

class BleedingComponent : public UnitStatusComponent
{
private:
	int m_maxDamagedCount = 3;			// 목표 출혈 데미지 횟수
	int m_currentDamagedCount = 0;			// 출혈로 데미지를 입은 횟수
	int m_bleedDamage = 1.0f;
	float m_bleedDuration = 5.0f;			// 출혈 지속 시간
	float m_bleedInterval = 1.0f;			// 출혈 피해 간격
	float m_bleedElapsed = 0.0f;			// 출혈 피해 경과 시간

	std::map<Unit*, StatusTimer*> opponentUnitMap;	// 현재 출혈을 겪는 유닛, 해당 유닛의 출혈 타이머

public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;
	
public:
	virtual void Update() override;
};

