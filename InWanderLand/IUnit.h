#pragma once
#include "Component.h"

/// <summary>
/// IUnit은 모든 캐릭터들의 공통 부모이다.
/// 모든 캐릭터들은 공통된 행동들을 가지고 있다.
/// </summary>

enum class State
{
	IDLE,
	MOVE,
	ATTACKMOVE,
	CHASE,
	ATTACK,
	DIE,
};

enum class UnitType
{
	PLAYER, // 임시
	WARRIOR,
	ARCHER,
};

enum class AttackType
{
	MELEE,
	RANGE,
};

class IUnit : public yunutyEngine::Component
{
public:
	virtual void Idle() = 0;
	virtual void Move() = 0;
	virtual void Attack() = 0;
	virtual void AttackMove() = 0;
	virtual void Chase() = 0;
	virtual void Damaged(float damage) = 0;
	virtual void Hit() = 0;
	virtual void Die() = 0;
};