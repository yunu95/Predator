#pragma once
#include "AttackSystem.h"

class RangedAttackSystem : public AttackSystem
{
private:
	float m_bulletSpeed;
	Unit* m_ownerUnit;
public:
	virtual void Attack(Unit* opponentUnit) override;

	void SetOwnerUnit(Unit* p_unit);
	void SetBulletSpeed(float speed);
};

