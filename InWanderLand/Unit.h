#pragma once
#include "YunutyEngine.h"
#include "FSM.h"
//#include <memory>
#include <list>

/// <summary>
/// 유닛들이 공유하는 멤버.
/// 기본적으로 모든 유닛은 BaseUnitEntity를 상속한다.
/// </summary>

class Unit : public Component
{
private:
	enum class UnitState
	{
		Idle,
		Move,
		Chase,
		Attack,
		AttackMove,
		Death,
		StateEnd
	};
	FSM<UnitState> unitFSM{UnitState::Idle};
	string unitType;

	float m_speed;
	float m_bulletSpeed;

	float chaseUpdateDelay;

	// 지금 수행중인 명령
	UnitState currentOrder=UnitState::Idle;

	float m_IdDistance;
	float m_AtkDistance;

	float idleToChaseDelay = 1.5f;
	float idleElapsed;

	float MoveUpdateDelay;

	float moveFunctionElapsed;
	float moveFunctionCallDelay = 0.1f;

	float chaseFunctionElapsed;
	float chaseFunctionCallDelay = 0.1f;

	float attackFunctionElapsed;
	float attackFunctionCallDelay = 0.3f;

	// 처음 인식 범위에 들어왔을 때 저장되는 상대 유닛.
	std::list<yunutyEngine::GameObject*> m_opponentGameObjectList;
	yunutyEngine::GameObject* m_currentTargetObject;
	Vector3d m_currentMovePosition;		// 현재 상대의 위치

private:
	/// 현재 상태에서 다른 상태로 가기 위한 bool값을 판별해주는 함수.
	/// 내부에서 fsm의 함수를 호출해야하니 자식들이 재정의 해줄 것!

	void IdleEngage();
	void MoveEngage();
	void AttackMoveEngage();
	void AttackEngage();
	void ChaseEngage();
	void DeathEngage();

	void IdleEngageFunction();
	void MoveEngageFunction();
	void AttackMoveEngageFunction();
	void ChaseEngageFunction();
	void AttackEngageFunction();
	void DeathEngageFunction();

	void IdleUpdate();
	void MoveUpdate();
	void AttackMoveUpdate();
	void ChaseUpdate();
	void AttackUpdate();

	void IdleUpdateFunction();
	void MoveUpdateFunction();
	void AttackMoveUpdateFunction();
	void ChaseUpdateFunction();
	void AttackUpdateFunction();

	void StopPosition();

public:
	virtual void Start() override;
	virtual void Update() override;

	string GetType() const;
	void SetType(string type);

	void SetIdRadius(float radius);
	void SetAtkRadius(float radius);
	void SetUnitSpeed(float speed);

	void OrderMove(Vector3d position);
	void OrderAttackMove(Vector3d position);
	void SetOpponentGameObject(yunutyEngine::GameObject* obj);
	void DeleteOpponentGameObject(yunutyEngine::GameObject* obj);
	void EnterIDRange(GameObject* opponentObject);
};

