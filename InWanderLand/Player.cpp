#include "Player.h"
#include "Dotween.h"
#include "RTSCam.h"

void Player::Start()
{
	unitType = "Player";
	m_speed = 2.0f;
	m_bulletSpeed = 5.1f;


	idleToChase = false;
	idleToAttack = false;
	chaseToIdle = false;
	attackToIdle = false;
	changeToMoveState = false;

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

	for (int i = 1; i < UnitState::StateEnd - 1; i++)
	{
		if (i != UnitState::Death)
		{
			unitFSM->transitions[UnitState(i)].push_back({ UnitState::Move,
			[this]() {return changeToMoveState == true; } });
		}
	}

	unitFSM->engageAction[UnitState::Idle] = [this]() { IdleEngage(); };
	unitFSM->engageAction[UnitState::Move] = [this]() { MoveEngage(); };
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
	if (!(m_opponentGameObjectList.empty()))
	{
		m_currentTargetObject = m_opponentGameObjectList.front();
		m_currentTargetPosition = m_currentTargetObject->GetTransform()->GetWorldPosition();
	}

	rtsCamComponent->groundLeftClickCallback = [=](Vector3d position)
	{
		m_movePosition = position;
		changeToMoveState = true;
	};
}

void Player::SetMovingSystemComponent(RTSCam* sys)
{
	rtsCamComponent = sys;
}

void Player::MoveEngage()
{
	changeToMoveState = false;
	MoveStateFunction();
}

void Player::MoveStateFunction()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 1, 1, 0 });

	GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(m_movePosition);

}

