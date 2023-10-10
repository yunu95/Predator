#pragma once
#include "IGrounded.h"
#include "YunuGIColor.h"
#include "Collision2D.h"
#include "Vector3.h"

class Zergling : public IGrounded
{
public:
	Zergling();
	virtual void Idle() override;
	virtual void Move() override;
	virtual void Attack() override;
	virtual void Chase() override;
	virtual void Die() override;

protected:
	virtual void Update() override;
	virtual void OnCollisionEnter2D(const yunutyEngine::Collision2D& collision) override;

private:
	bool m_isMove;
	float m_moveSpeed;
	float m_attackPower;
	float m_attackSpeed;
};