#include "Player.h"
#include "Dotween.h"
#include "RTSCam.h"

void Player::Start()
{
	unitType = "Player";
	m_speed = 1.5f;
	m_bulletSpeed = 5.1f;
	distanceOffset = 0.25f;

	idleToChase = false;
	idleToAttack = false;
	chaseToIdle = false;
	attackToIdle = false;
	changeToMove = false;
	moveToIdle = false;

	playerFSM = new FSM<PlayerState>(PlayerState::Idle);
	//IdleEngage();		// 최초 실행 시 상태함수 호출 안함... 그래서 일단 호출 직접 해줬다.

	playerFSM->transitions[PlayerState::Idle].push_back({ PlayerState::Chase,
		[this]() { return idleToChase == true; } });

	playerFSM->transitions[PlayerState::Idle].push_back({ PlayerState::Attack,
		[this]() { return idleToChase == true; } });

	playerFSM->transitions[PlayerState::Move].push_back({ PlayerState::Idle,
		[this]() { return moveToIdle == true; } });

	playerFSM->transitions[PlayerState::Chase].push_back({ PlayerState::Idle,
		[this]() { return chaseToIdle == true; } });

	playerFSM->transitions[PlayerState::Chase].push_back({ PlayerState::Attack,
		[this]() { return idleToAttack == true; } });

	playerFSM->transitions[PlayerState::Attack].push_back({ PlayerState::Idle,
		[this]() { return attackToIdle == true; } });

	for (int i = 0; i < PlayerState::StateEnd; i++)
	{
		if (i == PlayerState::Death)
		{
			break;
		}
		else
		{
			playerFSM->transitions[PlayerState(i)].push_back({ PlayerState::Move,
			[this]() {return changeToMove == true; } });
		}
	}

	playerFSM->engageAction[PlayerState::Idle] = [this]() { IdleEngage(); };
	playerFSM->engageAction[PlayerState::Chase] = [this]() { ChaseEngage(); };
	playerFSM->engageAction[PlayerState::Move] = [this]() { MoveEngage(); };
	playerFSM->engageAction[PlayerState::Attack] = [this]() { AttackEngage(); };

	playerFSM->updateAction[PlayerState::Chase] = [this]() { ChaseUpdate(); };
	playerFSM->updateAction[PlayerState::Attack] = [this]() { AttackUpdate(); };
	playerFSM->updateAction[PlayerState::Move] = [this]() { MoveUpdate(); };


	GetGameObject()->GetComponent<NavigationAgent>()->SetSpeed(m_speed);
}

void Player::Update()
{
	playerFSM->UpdateState();

	if (!(m_opponentGameObjectList.empty()))
	{
		m_currentTargetObject = m_opponentGameObjectList.front();
		m_currentTargetPosition = m_currentTargetObject->GetTransform()->GetWorldPosition();
	}

	rtsCamComponent->groundLeftClickCallback = [=](Vector3d position)
	{
		m_movePosition = position;
		isMoveStarted = false;
		changeToMove = true;
	};

	if (!tempCheck)
	{
		rtsCamComponent->groundRightClickCallback = [=](Vector3d tempposition)
		{
			m_tempPosition = tempposition;
			GetGameObject()->GetTransform()->SetWorldPosition(m_tempPosition);
			tempCheck = true;
		};
	}

}

void Player::OnDestroy()
{
	delete playerFSM;
}

void Player::IdleToOtherStatesTransition()
{

}

void Player::ChaseTransition()
{

}

void Player::AttackToIdleTransition()
{

}

void Player::IdleEngage()
{
	moveToIdle = false;
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 0, 0 });
}

void Player::ChaseEngage()
{

}

void Player::AttackEngage()
{

}

void Player::IdleEngageFunction()
{

}

void Player::AttackEngageFunction()
{

}

void Player::ChaseEngageFunction()
{

}

void Player::IdleUpdate()
{

}

void Player::ChaseUpdate()
{

}

void Player::AttackUpdate()
{

}

void Player::SetMovingSystemComponent(RTSCam* sys)
{
	rtsCamComponent = sys;
}

void Player::MoveEngage()
{
	changeToMove = false;
	m_previousPosition = GetGameObject()->GetTransform()->GetWorldPosition();
	MoveEngageFunction();
}

void Player::MoveEngageFunction()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 1, 1, 0 });

	GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(m_movePosition);

	

}

void Player::MoveUpdate()
{
	// 상태 진입 시 GameObject의 위치를 매 프레임 마다 이전 프레임과 비교하기.
	// 최초 진입 생각하기. MoveTo 호출 전에 비교하면 당연히 같다.

	float distance = (GetGameObject()->GetTransform()->GetWorldPosition() - m_movePosition).Magnitude();

	if (!isMoveStarted && GetGameObject()->GetTransform()->GetWorldPosition() != m_previousPosition)
	{
		isMoveStarted = true;
	}
	else if (isMoveStarted && distance < distanceOffset)
	{
		GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(GetGameObject()->GetTransform()->GetWorldPosition());
		moveToIdle = true;
		isMoveStarted = false;
	}
	m_previousPosition = GetGameObject()->GetTransform()->GetWorldPosition();
}

