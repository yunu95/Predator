#pragma once
#include "YunutyEngine.h"
/// <summary>
/// 상태이상과 버프 상태 컴포넌트. ex) 출혈, 넉백...
/// Collider가 붙은 (kinematic == true) 오브젝트에 붙여주는 컴포넌트
/// 충돌 시 OnTrigger~ 함수가 호출된다.
/// </summary>

class Unit;

class UnitStatusComponent : public Component
{
public:
	virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) = 0;
	virtual void OnTriggerEnter(physics::Collider* collider) override;
};

