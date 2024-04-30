#pragma once
#include "StatusEffect.h"
#include <unordered_map>

class StatusTimer;

class BleedingComponent : public StatusEffect
{
private:
	// 피해를 받는 유닛이 갖고 있어야하는 정보
	struct bleededUnitInfo
	{
		int currentBleedingStack = 1;					// 현재 출혈 스택
		Unit* bleedingUnit;
		int currentDamagedCount = 0;
		StatusTimer* statusTimer;
	};

	int maxBleedingStack = 3;						// 최대 출혈 스택

	const int m_maxDamageCount = 5;						// 1스택 당 받을 출혈 데미지 카운트

	int m_bleedDamage = 1;
	float m_bleedDuration = 1.0f;					// 1회 출혈 데미지 간격 시간
	float m_bleedElapsed = 0.0f;					// 출혈 피해 경과 시간

	//std::map<bleededUnitInfo*, StatusTimer*> opponentUnitMap;	// 현재 출혈을 겪는 유닛, 해당 유닛의 출혈 타이머
	std::unordered_map< Unit*,std::unique_ptr<bleededUnitInfo> > opponentUnits;

public:
	bool isActivated = true;

	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;
	
public:
	virtual void Update() override;
};

