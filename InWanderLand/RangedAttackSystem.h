#pragma once
#include "AttackSystem.h"
/// <summary>
/// 원거리 공격은 다른 공격과는 달리 이미 오브젝트가 만들어진 풀에서 오브젝트를 불러온다.
/// 또한 유닛에게 종속되는 개념이 아니다.
/// </summary>
class RangedAttackSystem : public AttackSystem
{
private:
	float m_bulletSpeed;
	float m_range;
	Unit* m_ownerUnit;
public:
	virtual void Attack(Unit* opponentUnit, float offset) override;

	void SetOwnerUnit(Unit* p_unit);
	void SetBulletSpeed(float speed);
	void SetRange(float p_rng);

public:
	virtual void Start() override;
};

