#pragma once
#include "YunutyEngine.h"
#include <list>

/// <summary>
/// 유닛들이 공유하는 멤버.
/// 기본적으로 모든 유닛은 BaseUnitEntity를 상속한다.
/// </summary>

class BaseUnitEntity : public Component
{
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
	std::list<yunutyEngine::GameObject*> m_opponentGameObjectList;
	yunutyEngine::GameObject* m_currentTargetObject;
	Vector3d m_currentTargetPosition;		// 현재 상대의 위치

protected:
	/// 현재 상태에서 다른 상태로 가기 위한 bool값을 판별해주는 함수.
	/// 내부에서 fsm의 함수를 호출해야하니 자식들이 재정의 해줄 것!
	virtual void ChaseTransition() = 0;
	virtual void AttackTransition() = 0;

	virtual void IdleEngage() = 0;
	virtual void ChaseEngage() = 0;
	virtual void AttackEngage() = 0;

	virtual void IdleEngageFunction() = 0;
	virtual void ChaseEngageFunction() = 0;
	virtual void AttackEngageFunction() = 0;

	virtual void IdleUpdate() = 0;
	virtual void ChaseUpdate() = 0;
	virtual void AttackUpdate() = 0;

public:
	virtual void IdleTransition() = 0;

	string GetType() const;

	void SetIdRadius(float radius);
	void SetAtkRadius(float radius);

	void InitFSM();

	void SetOpponentGameObject(yunutyEngine::GameObject* obj);
	void DeleteOpponentGameObject(yunutyEngine::GameObject* obj);
	void EnterIDRange();
};

