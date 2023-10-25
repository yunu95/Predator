#pragma once
#include "IUnit.h"
#include "YunuGIColor.h"
#include "Collision2D.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Curve.h"
#include "Timer.h"
#include <list>

class GameObject;

class CommonUnit : public IUnit
{
public:
	CommonUnit();
	virtual void Idle() override;
	virtual void Move() override;
	virtual void Attack() override;
	virtual void AttackMove() override;
	virtual void Chase() override;
	virtual void Damaged(float damage) override;
	virtual void Hit() override;
	virtual void Die() override;

	void Initialize(UnitType unitType);
	void DeleteDeadObjectInList(CommonUnit* unit);

	// Setter
	void SetDestination(Vector3d targetPos) { lastMouseScreenPos = targetPos; }
	void SetPlayer(bool isPlayer) { m_isPlayer = isPlayer; }
	void SetHP(float hp) { m_hp = hp; }
	// Getter
	float GetHP() { return m_hp; }
	float GetDetectRange() { return m_detectRange; }
	float GetAttackRange() { return m_attackRange; }
	bool GetIsPlayer() { return m_isPlayer; }
	float GetHitBoxWidth() { return m_hitboxWidth; }
	float GetHitBoxHeight() { return m_hitboxHeight; }

protected:
	virtual void Update() override;
	virtual void OnCollisionEnter2D(const yunutyEngine::Collision2D& collision) override;
	virtual void OnCollisionExit2D(const yunutyEngine::Collision2D& collision) override;

private:
	void moveToDestination(yunutyEngine::Vector3d targetPos);

	UnitType m_unitType;
	AttackType m_attackType;
	State m_state;
	bool m_isDying;
	bool m_isPlayer;
	bool m_isAttackReady;
	float m_maxHp;
	float m_hp;
	float m_mp;
	float m_defense;
	float m_moveSpeed;
	float m_attackPower;
	float m_attackSpeed;
	float m_attackCoolDown;
	float m_attackRange;
	float m_detectRange;
	float m_hitboxWidth;
	float m_hitboxHeight;
	float m_projectileSpeed;
	yunutyEngine::Vector2d lastMouseScreenPos;

	//const yunutyEngine::math::Curve m_moveCurve{ [](double t) {return 1 - (1 - t) * (1 - t); } };
	//const yunutyEngine::math::Curve m_moveCurve{ [](double t) {return  t < 0.5 ? 2 * t * t : 1 - (-2 * t + 2) * (-2 * t + 2) / 2; } };
	
	WanderUtil::Timer attackTimer;
	WanderUtil::Timer attackCoolDownTimer;

	std::list<CommonUnit*> detectedEnemies;
	std::list<CommonUnit*> attackRangeEnemies;
	CommonUnit* targetEnemy;
	yunutyEngine::GameObject* hpBarObject;
};