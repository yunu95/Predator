#pragma once
#include "YunutyEngine.h"
#include "Unit.h"

#define LENGTH_UNIT 3

/// <summary>
/// UnitFactory에서의 유닛 생산을 좀 더 효율적으로 하기 위한 클래스.
/// </summary>

class UnitProductor : public Component
{
protected:


	// 임시. 모델링 파일로 대체해주기
	GameObject* m_unitGameObject;
	Unit* m_unitComponent;

	Unit::BaseUnitAnimationStruct m_baseUnitAnimations;

	NavigationField* m_navField;

public:
	string m_objectName;
	Unit::UnitType m_unitType;
	Unit::UnitSide m_unitSide;

	float m_healthPoint;
	int m_manaPoint;

	float m_autoAttackDamage;
	int m_criticalHitProbability;				// 치명타 확률
	float m_criticalHitMultiplier;				// 공격 시 치명타 피해량

	/// Decreasing Damage Elements
	int m_defensePoint;
	int m_dodgeProbability;					// 회피율
	float m_criticalDamageDecreaseMultiplier;	// 피격 시 치명타 피해 감소 배율

	float m_idRadius;
	float m_atkRadius;
	float m_unitSpeed;

	float m_attackDelay;

	virtual GameObject* CreateUnit(Vector3d startPos) = 0;

	void SetCommonComponents();

	virtual void SetUnitData() = 0;

	virtual void SetPlayerRelatedComponents(Unit* playerUnit);
};

