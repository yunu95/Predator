#pragma once
#include "AttackSystem.h"
/// <summary>
///
/// </summary>
class MeleeAttackSystem : public AttackSystem
{
private:
	GameObject* meleeAttackColliderObject;
	GameObject* meleeAttackColliderDebugObject;
	float attackColliderRemainTime;

	float colliderRemainElasped = 0.f;
	bool colliderActivated = false;
public:
	virtual void Attack(Unit* opponentUnit) override;
	void SetColliderObject(GameObject* colliderObj);
	void SetColliderDebugObject(GameObject* debugobj);
	void SetColliderRemainTime(float time);
public:
	virtual void Start() override;
	virtual void Update() override;
};  

