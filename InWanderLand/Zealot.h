#pragma once
#include "IGrounded.h"
#include "YunuGIColor.h"
#include "Collision2D.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Curve.h"

class Zealot : public IGrounded
{
public:
	Zealot();
	virtual void Idle() override;
	virtual void Move() override;
	virtual void Attack() override;
	virtual void Chase() override;
	virtual void Die() override;

	void SetDestination(Vector3d targetPos) { lastMouseScreenPos = targetPos; }

protected:
	virtual void Update() override;
	virtual void OnCollisionEnter2D(const yunutyEngine::Collision2D& collision) override;

private:
	void moveToDestination(Vector3d targetPos);

	State m_state;
	bool m_isMove;
	bool m_isChase;
	bool m_isAttack;
	float m_moveSpeed;
	float m_attackPower;
	float m_attackSpeed;
	float m_attackRange;
	yunutyEngine::Vector3d targetEnemyPos;
	yunutyEngine::Vector2d lastMouseScreenPos;

	const yunutyEngine::math::Curve m_moveCurve{ [](double t) {return 1 - (1 - t) * (1 - t); } };
	float m_moveCurveDuration;
	float m_moveCurveElapsed;
};