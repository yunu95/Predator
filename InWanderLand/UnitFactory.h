#pragma once
#include "YunutyEngine.h"

class Unit;
class RangeSystem;

enum class UnitType
{
	PLAYER,
	ENEMY,
	BOSS
};

class UnitFactory
{
private:
	GameObject* m_unitGameObject;
	GameObject* m_rangeGameObject;
	// Create하고 나서 Component public 함수를 호출해야하기 때문에 멤버 갖고 있기.
	// GetComponent로 설정하는 방법도 있긴 한데... 뭐가 좋을까?
	Unit* m_unit;
	yunutyEngine::graphics::StaticMeshRenderer* m_staticMeshRenderer;
	CircleCollider2D* m_circleCollider;
	NavigationAgent* m_navigationAgent;
	RangeSystem* m_rangeSystem;

	void AddUnitComponent(UnitType enumType);
	void AddMeshComponent(UnitType enumType);
	void AddColliderComponent();
	void AddNavigationComponent();
	void AddRangeSystemComponent();

private:
	float defaultUnitColliderRadius = 1.0f;
	
	float defaultIdRadius = 1.0f;
	float defalutAtkRadius = 2.0f;

	float defaultNavSpeed = 2.0f;
	float defaultRadius = 0.5f;


public:
	GameObject* CreateUnit(UnitType enumType);

	void SetUnitColliderRadius(float rad);
	void SetRangeRadius(float idRadius, float atkRadius);
	void SetNavField(NavigationField* field);
};

