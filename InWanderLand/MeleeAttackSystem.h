#pragma once
#include "AttackSystem.h"

enum class MeleeAttackType
{
	Collider,
	DirectAttack
};

class SpecialEffect;

/// <summary>
///
/// </summary>
class MeleeAttackSystem : public AttackSystem
{
private:
	MeleeAttackType m_meleeAttackType;

	GameObject* meleeAttackColliderObject;
	GameObject* meleeAttackColliderDebugObject;
	GameObject* ownerUnitObject;
	float attackColliderRemainTime;
	float colliderRemainElasped = 0.f;
	bool colliderActivated = false;

	SpecialEffect* m_specialEffect;
	float m_attackDamage;
	Unit* m_unitComponent;

public:
	virtual void Attack(Unit* opponentUnit, float offSet) override;
	void SetMeleeAttackType(MeleeAttackType p_type);
	void SetOwnerUnitObject(GameObject* unitobj);

	// Collider를 띄우고 없애는 식이라면 아래 함수 사용.
	void SetColliderObject(GameObject* colliderObj);
	void SetColliderDebugObject(GameObject* debugobj);
	void SetColliderRemainTime(float time);

	// 유닛의 Damaged()를 직접 호출하는 식이라면 아래 함수 사용.
	void SetDirectAttackSpecialEffect(SpecialEffect* p_effect);
	void SetDamage(float p_dmg);

public:
	virtual void Start() override;
	virtual void Update() override;
};  

