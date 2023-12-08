#include "Unit.h"
#include "Dotween.h"
#include "ProjectileSystem.h"

void Unit::Start()
{
	m_speed = 2.0f;
	m_bulletSpeed = 5.1f;

	chaseUpdateDelay = 0.1f;


	unitFSM.transitions[UnitState::Idle].push_back({ UnitState::Move,
		[this]() { return currentOrder == UnitState::Move; } });

	unitFSM.transitions[UnitState::Idle].push_back({ UnitState::AttackMove,
		[this]() { return currentOrder == UnitState::AttackMove; } });

	unitFSM.transitions[UnitState::Idle].push_back({ UnitState::Chase,
		[this]() { return ((!m_opponentGameObjectList.empty()) && idleElapsed >= idleToChaseDelay); } });

	unitFSM.transitions[UnitState::Move].push_back({ UnitState::Idle,
		[this]() { return (GetGameObject()->GetTransform()->GetWorldPosition() - m_currentMovePosition).Magnitude() < 0.3f; } });

	unitFSM.transitions[UnitState::Move].push_back({ UnitState::AttackMove,
		[this]() { return currentOrder == UnitState::AttackMove; } });

	unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Idle,
		[this]() { return (GetGameObject()->GetTransform()->GetWorldPosition() - m_currentMovePosition).Magnitude() < 0.3f; } });

	unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Move,
		[this]() { return currentOrder == UnitState::Move; } });

	unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Chase,
		[this]() { return !m_opponentGameObjectList.empty(); } });

	unitFSM.transitions[UnitState::Chase].push_back({ UnitState::Idle,
		[this]() { return m_opponentGameObjectList.empty(); } });

	unitFSM.transitions[UnitState::Chase].push_back({ UnitState::Attack,
		[this]() { return (GetGameObject()->GetTransform()->GetWorldPosition() - m_currentTargetObject->GetTransform()->GetWorldPosition()).Magnitude() <= m_AtkDistance + 0.4f; } });

	unitFSM.transitions[UnitState::Attack].push_back({ UnitState::Idle,
		[this]() { return (GetGameObject()->GetTransform()->GetWorldPosition() - m_currentTargetObject->GetTransform()->GetWorldPosition()).Magnitude() >= m_AtkDistance + 0.4f; } });

	unitFSM.transitions[UnitState::Attack].push_back({ UnitState::Move,
		[this]() { return currentOrder == UnitState::Move; } });

	unitFSM.engageAction[UnitState::Idle] = [this]() { IdleEngage(); };
	unitFSM.engageAction[UnitState::Move] = [this]() { MoveEngage(); };
	unitFSM.engageAction[UnitState::AttackMove] = [this]() { AttackMoveEngage(); };
	unitFSM.engageAction[UnitState::Chase] = [this]() { ChaseEngage(); };
	unitFSM.engageAction[UnitState::Attack] = [this]() { AttackEngage(); };
	unitFSM.engageAction[UnitState::Death] = [this]() { DeathEngage(); };

	unitFSM.updateAction[UnitState::Idle] = [this]() { IdleUpdate(); };
	unitFSM.updateAction[UnitState::Move] = [this]() { MoveUpdate(); };
	unitFSM.updateAction[UnitState::AttackMove] = [this]() { AttackMoveUpdate(); };
	unitFSM.updateAction[UnitState::Chase] = [this]() { ChaseUpdate(); };
	unitFSM.updateAction[UnitState::Attack] = [this]() { AttackUpdate(); };
}

void Unit::Update()
{
	unitFSM.UpdateState();

}

void Unit::IdleEngage()
{
	currentOrder = UnitState::Idle;
	idleElapsed = 0.0f;
	//ResetTransitionTrigger();
	IdleEngageFunction();
}

void Unit::IdleEngageFunction()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 0, 0 });

	if (unitFSM.previousState == UnitState::Attack)
		idleToChaseDelay = 1.0f;
	else
		idleToChaseDelay = 0.0f;

	StopPosition();
}

void Unit::IdleUpdate()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 0, 0 });
	
	idleElapsed += Time::GetDeltaTime();

	IdleUpdateFunction();
	// 데미지를 입으면 공격한 상대의 정보를 list에 등록하고 쫓아가기
}


void Unit::MoveEngage()
{
	currentOrder = UnitState::Move;

	moveFunctionElapsed = 0.0f;

	GetGameObject()->GetComponent<NavigationAgent>()->SetSpeed(m_speed);

	MoveEngageFunction();
}


void Unit::MoveUpdate()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 1, 0, 0 });

	moveFunctionElapsed += Time::GetDeltaTime();

	if (moveFunctionElapsed >= moveFunctionCallDelay)
		MoveUpdateFunction();
}

void Unit::AttackMoveEngage()
{
	currentOrder = UnitState::AttackMove;

	moveFunctionElapsed = 0.0f;
	
	GetGameObject()->GetComponent<NavigationAgent>()->SetSpeed(m_speed);

	AttackMoveEngageFunction();
}

void Unit::AttackMoveUpdate()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0.4, 0.3, 0.25, 0 });

	moveFunctionElapsed += Time::GetDeltaTime();

	if (moveFunctionElapsed >= moveFunctionCallDelay)
		AttackMoveUpdateFunction();
}

void Unit::AttackEngage()
{
	currentOrder = UnitState::Attack;

	attackFunctionElapsed = 0.0f;

	AttackEngageFunction();
}

void Unit::AttackUpdate()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 1, 0 });

	attackFunctionElapsed += Time::GetDeltaTime();

	if (attackFunctionElapsed >= attackFunctionCallDelay)
		AttackUpdateFunction();
}

void Unit::ChaseEngage()
{
	currentOrder = UnitState::Chase;

	ChaseEngageFunction();
}

void Unit::ChaseUpdate()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0.5, 0.2, 0, 0 });
	chaseFunctionElapsed += Time::GetDeltaTime();

	if (chaseFunctionElapsed >= chaseFunctionCallDelay)
		ChaseUpdateFunction();
}

void Unit::DeathEngage()
{

}

void Unit::MoveEngageFunction()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 1, 0, 0 });
}

void Unit::AttackMoveEngageFunction()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0.4, 0.3, 0.25, 0 });
}

void Unit::ChaseEngageFunction()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0.5, 0.2, 0, 0 });
}

void Unit::AttackEngageFunction()
{
	// 제자리에 서게 하고,
	StopPosition();
	// 상대방을 공격하는 로직. 근접 공격 or 원거리 (투사체) 공격
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 1, 0 });

}

void Unit::DeathEngageFunction()
{

}

void Unit::IdleUpdateFunction()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 0, 0 });
}
/// 마우스로 찍은 위치로 이동하거나, 적을 쫓아가거나
void Unit::MoveUpdateFunction()
{
	moveFunctionElapsed = 0.0f;

	// 마우스로 찍은 위치로 이동한다.
	GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(m_currentMovePosition);
}

void Unit::AttackMoveUpdateFunction()
{
	moveFunctionElapsed = 0.0f;

	// 마우스로 찍은 위치로 이동한다.
	GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(m_currentMovePosition);
}

void Unit::ChaseUpdateFunction()
{
	chaseFunctionElapsed = 0.0f;

	bool isDistanceComparingStarted = false;

	float tempShortestDistance = 0.0f;

	for (auto e : m_opponentGameObjectList)
	{
		float distance = (GetGameObject()->GetTransform()->GetWorldPosition() - e->GetTransform()->GetWorldPosition()).Magnitude();

		if (!isDistanceComparingStarted || tempShortestDistance > distance)
		{
			tempShortestDistance = distance;
			m_currentTargetObject = e;
			isDistanceComparingStarted = true;
		}
	}

	// 거리를 구한 후, 공격할건지 판단.
	//if ((GetGameObject()->GetTransform()->GetWorldPosition() - m_currentTargetObject->GetTransform()->GetWorldPosition()).Magnitude() < m_AtkDistance + 0.4f)
	//	chaseToAttack = true;

	// 마우스로 찍은 위치로 이동한다.
	GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(m_currentTargetObject->GetTransform()->GetWorldPosition());
}

void Unit::AttackUpdateFunction()
{
	attackFunctionElapsed = 0.0f;

	ProjectileSystem::GetInstance()->Shoot(this, m_currentTargetObject->GetComponent<Unit>(), m_bulletSpeed);
}

void Unit::StopPosition()
{
	GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(GetGameObject()->GetTransform()->GetWorldPosition());
}

std::string Unit::GetType() const
{
	return unitType;
}

void Unit::SetType(string type)
{
	unitType = type;
}

void Unit::SetIdRadius(float radius)
{
	m_IdDistance = radius;
}

void Unit::SetAtkRadius(float radius)
{
	m_AtkDistance = radius;
}

void Unit::SetUnitSpeed(float speed)
{
	m_speed = speed;
}

void Unit::OrderMove(Vector3d position)
{
	m_currentMovePosition = position;
	currentOrder = UnitState::Move;
	/*
	idleToMove = true;
	attackToMove = true;
	if (unitFSM.currentState == UnitState::Attack)
		isMoveCalledWhileAttack = true;*/
}
void Unit::OrderAttackMove(Vector3d position)
{
	m_currentMovePosition = position;
	currentOrder = UnitState::AttackMove;
	/*
	idleToMove = true;
	attackToMove = true;
	if (unitFSM.currentState == UnitState::Attack)
		isMoveCalledWhileAttack = true;*/
}

void Unit::SetOpponentGameObject(yunutyEngine::GameObject* obj)
{
	m_opponentGameObjectList.push_back(obj);
	if (m_currentTargetObject == nullptr)
		m_currentTargetObject = obj;
}

void Unit::DeleteOpponentGameObject(yunutyEngine::GameObject* obj)
{
	m_opponentGameObjectList.remove(obj);
	if (m_currentTargetObject == obj)
		m_currentTargetObject = nullptr;
}

void Unit::EnterIDRange(GameObject* opponentObject)
{
	m_currentMovePosition = opponentObject->GetTransform()->GetWorldPosition();
	currentOrder = UnitState::Chase;
	//moveToChase = true;
	//idleToChase = true;
	//attackToMove = true;

}
