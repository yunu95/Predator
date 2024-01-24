#pragma once
#include "YunutyEngine.h"
#include "Unit.h"
/// <summary>
/// UnitFactory에서의 유닛 생산을 좀 더 효율적으로 하기 위한 클래스.
/// </summary>

class UnitProductionOrder : public Component
{
protected:
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

	// 임시. 모델링 파일로 대체해주기
	GameObject* m_unitGameObject;
	Unit* m_unitComponent;

	Unit::BaseUnitAnimationStruct m_baseUnitAnimations;

	NavigationField* m_navField;
	Vector3d m_startPosition;

public:
	virtual GameObject* CreateUnitWithOrder();

	virtual void SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition) = 0;

	virtual void SetPlayerRelatedComponents(Unit* playerUnit);
};

