#include "Player.h"
#include "Dotween.h"

void Player::Start()
{
	unitType = "Player";
	m_speed = 3.0f;
	m_bulletSpeed = 5.1f;
	idleToChase = false;
	idleToAttack = false;
	chaseToIdle = false;
	attackToIdle = false;

	unitFSM = new FSM<UnitState>(UnitState::Idle);
	IdleEngage();		// 최초 실행 시 상태함수 호출 안함... 그래서 일단 호출 직접 해줬다.

	unitFSM->transitions[UnitState::Idle].push_back({ UnitState::Chase,
		[this]() { return idleToChase == true; } });

	unitFSM->transitions[UnitState::Idle].push_back({ UnitState::Attack,
		[this]() { return idleToChase == true; } });

	unitFSM->transitions[UnitState::Chase].push_back({ UnitState::Idle,
		[this]() { return chaseToIdle == true; } });

	unitFSM->transitions[UnitState::Chase].push_back({ UnitState::Attack,
		[this]() { return idleToAttack == true; } });

	unitFSM->transitions[UnitState::Attack].push_back({ UnitState::Idle,
		[this]() { return attackToIdle == true; } });

	unitFSM->engageAction[UnitState::Idle] = [this]() { IdleEngage(); };
	unitFSM->engageAction[UnitState::Chase] = [this]() { ChaseEngage(); };
	unitFSM->engageAction[UnitState::Attack] = [this]() { AttackEngage(); };

	unitFSM->updateAction[UnitState::Chase] = [this]() { ChaseUpdate(); };
	unitFSM->updateAction[UnitState::Attack] = [this]() { AttackUpdate(); };
}

void Player::Update()
{
	unitFSM->UpdateState();

	// 인식 범위 내에 들어오게 된다면, 목표로 하는(적군) 오브젝트의 위치 정보를 계속 받아와야한다.
	// 그렇게 되면 이동중에 해당 적군 오브젝트의 위치가 바뀌어도 그에 맞게 자연스럽게 이동할 수 있지 않을까?
	if (m_opponentGameobject != nullptr)
		m_opponentPosition = m_opponentGameobject->GetTransform()->GetWorldPosition();
}

