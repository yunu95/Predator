#pragma once
#include "YunutyEngine.h"
#include "Unit.h"
/// <summary>
/// UnitFactory에서의 유닛 생산을 좀 더 효율적으로 하기 위한 클래스.
/// </summary>

using namespace yunuGI;

class UnitProductionOrder : public Component
{
protected:
	string m_objectName;
	Unit::UnitType m_unitType;
	Unit::UnitSide m_unitSide;
	int m_hp;
	int m_ap;
	float m_idRadius;
	float m_atkRadius;
	float m_unitSpeed;

	// 임시. 모델링 파일로 대체해주기
	GameObject* m_unitGameObject;
	GameObject* m_unitAttackColliderObject;
	IAnimation* m_idleAnimation;
	IAnimation* m_walkAnimation;
	IAnimation* m_attackAnimation;
	IAnimation* m_deathAnimation;
	NavigationField* m_navField;
	Vector3d m_startPosition;

public:
	virtual GameObject* CreateUnitWithOrder();

	virtual void SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition) = 0;

	virtual void SetPlayerRelatedComponents(Unit* playerUnit);
};

