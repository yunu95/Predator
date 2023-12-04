#pragma once
#include "YunutyEngine.h"
#include "FSM.h"
#include <list>
#include "BaseUnitEntity.h"

/// <summary>
/// 일반 적군 유닛 클래스이자 플레이어 및 보스 클래스의 부모 클래스.
/// 기본적으로 모든 유닛의 기능을 갖고 있다.
/// </summary>

class Timer;
class ProjectileSystem;
class Projectile;

class Enemy : public BaseUnitEntity
{
private:
	enum UnitState
	{
		Idle,
		Move,
		Chase,
		Detect,
		Attack,
		Death,
		StateEnd
	};

	FSM<UnitState>* unitFSM;

private:
	/// 현재 상태에서 다른 상태로 가기 위한 bool값을 판별해주는 함수.
	void IdleTransition() override;
	void ChaseTransition() override;
	void AttackTransition() override;

	virtual void IdleEngage() override;
	virtual void ChaseEngage() override;
	virtual void AttackEngage() override;

	virtual void IdleEngageFunction() override;
	virtual void AttackEngageFunction() override;
	virtual void ChaseEngageFunction() override;

	virtual void IdleUpdate() override;
	virtual void ChaseUpdate() override;
	virtual void AttackUpdate() override;

public:
	// 컴포넌트에서 가져온 함수
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDestroy() override;
};

