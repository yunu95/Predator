#pragma once
#include "YunutyEngine.h"
#include "FSM.h"

/// <summary>
/// 일반 적군 유닛 클래스이자 플레이어 및 보스 클래스의 부모 클래스.
/// 기본적으로 모든 유닛이 갖는 기능을 갖고 있다.
/// </summary>

class Timer;
class ProjectileSystem;
class Projectile;

class Unit : public Component
{
private:
	enum UnitState
	{
		Idle,
		Move,
		Chase,
		Detect,
		Attack,
		StateEnd
	};

	FSM<UnitState>* unitFSM;

protected:
	float m_speed;
	float m_bulletSpeed;

	bool isInIDRange;
	bool isInAtkRange;
	bool isOutAtkRange;
	bool isOutIDRange;

	float m_IdDistance;
	float m_AtkDistance;

	// 처음 인식 범위에 들어왔을 때 저장되는 상대 유닛.
	GameObject* m_opponentGameobject;
	Vector3d m_opponentPosition;

	void IdleEngage();
	void ChaseEngage();
	void AttackEngage();

	void ChaseUpdate();
	void AttackUpdate();

	void AttackTransition();

	void IdleFunction();
	void AttackFunction();
	void ChaseFunction();

public:
	// 컴포넌트에서 가져온 함수
	virtual void Start() override;
	virtual void Update() override;

public:
	// 자식들에게 물려주되, 외부에서 호출이 가능한 함수
	string GetType() const;
	void SetIdRadius(float radius);
	void SetAtkRadius(float radius);

	void IdentifyTransition();
	void ExitIDRangeTransition();
	void SetOpponentGameObject(GameObject* obj);

protected:
	string unitType;

};

