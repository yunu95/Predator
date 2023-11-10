#pragma once
#include "YunutyEngine.h"
#include "FSM.h"
#include <list>

/// <summary>
/// 일반 적군 유닛 클래스이자 플레이어 및 보스 클래스의 부모 클래스.
/// 기본적으로 모든 유닛의 기능을 갖고 있다.
/// </summary>

class Timer;
class ProjectileSystem;
class Projectile;

class Unit : public Component
{
protected:
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

protected:
	string unitType;

	float m_speed;
	float m_bulletSpeed;

	bool idleToChase;
	bool idleToAttack;
	bool attackToIdle;
	bool chaseToIdle;

	bool initToIdle;

	bool isJustEntered;			// 밖에 있다가 들어온 경우. 

	bool isDistanceComparingStarted;

	float m_IdDistance;
	float m_AtkDistance;

	float transitionDelay;

	float chaseUpdateDelay;

	// 처음 인식 범위에 들어왔을 때 저장되는 상대 유닛.
	std::list<GameObject*> m_opponentGameObjectList;
	GameObject* m_currentTargetObject;
	Vector3d m_currentTargetPosition;		// 현재 상대의 위치

	void IdleEngage();
	void ChaseEngage();
	void AttackEngage();

	void IdleUpdate();
	void ChaseUpdate();
	void AttackUpdate();

	void IdleEngageFunction();
	void AttackEngageFunction();
	void ChaseEngageFunction();

public:
	// 컴포넌트에서 가져온 함수
	virtual void Start() override;
	virtual void Update() override;

public:
	// 자식들에게 물려주되, 외부에서 호출이 가능한 함수
	string GetType() const;
	
	void SetIdRadius(float radius);
	void SetAtkRadius(float radius);


	/// 현재 상태에서 다른 상태로 가기 위한 bool값을 판별해주는 함수.
	void IdleTransition();
	void ChaseTransition();
	void AttackTransition();

	void InitFSM();

	void SetOpponentGameObject(GameObject* obj);
	void DeleteOpponentGameObject(GameObject* obj);
	void EnterIDRange();
};

