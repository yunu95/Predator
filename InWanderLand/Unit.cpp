#include "Unit.h"
#include "ProjectileSystem.h"

void Unit::Start()
{
	m_speed = 2.0f;
	m_bulletSpeed = 5.1f;

	chaseUpdateDelay = 0.1f;

	unitFSM.transitions[UnitState::Idle].push_back({ UnitState::Move,
		[this]() { return currentOrder == UnitState::Move; } });

	unitFSM.transitions[UnitState::Idle].push_back({ UnitState::AttackMove,
		[this]() { return currentOrder == UnitState::AttackMove || (unitFSM.previousState == UnitState::Attack && isAttackMoving); } });

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
		[this]() { return (GetGameObject()->GetTransform()->GetWorldPosition() - m_currentTargetObject->GetTransform()->GetWorldPosition()).Magnitude() <= m_atkDistance + 0.4f; } });

	unitFSM.transitions[UnitState::Attack].push_back({ UnitState::Idle,
		[this]() { return (GetGameObject()->GetTransform()->GetWorldPosition() - m_currentTargetObject->GetTransform()->GetWorldPosition()).Magnitude() >= m_atkDistance + 0.4f ||
		(m_currentTargetObject->GetComponent<Unit>()->GetUnitCurrentState() == UnitState::Death); } });

	unitFSM.transitions[UnitState::Attack].push_back({ UnitState::Move,
		[this]() { return currentOrder == UnitState::Move; } });

	for (int i = static_cast<int>(UnitState::Idle); i < static_cast<int>(UnitState::Death); i++)
	{
		unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Death,
		[this]() { return m_hp <= 0; } });
	}

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
	unitFSM.updateAction[UnitState::Death] = [this]() { DeathUpdate(); };
}

void Unit::Update()
{
	unitFSM.UpdateState();
}

#pragma region State Engage()
	void Unit::IdleEngage()
	{
		currentOrder = UnitState::Idle;
		idleElapsed = 0.0f;
		IdleEngageFunction();
	}

	void Unit::MoveEngage()
	{
		currentOrder = UnitState::Move;

		moveFunctionElapsed = 0.0f;

		GetGameObject()->GetComponent<NavigationAgent>()->SetSpeed(m_speed);

		MoveEngageFunction();
	}

	void Unit::AttackMoveEngage()
	{
		currentOrder = UnitState::AttackMove;

		moveFunctionElapsed = 0.0f;
	
		GetGameObject()->GetComponent<NavigationAgent>()->SetSpeed(m_speed);

		AttackMoveEngageFunction();
	}

	void Unit::AttackEngage()
	{
		currentOrder = UnitState::Attack;

		attackFunctionElapsed = 0.0f;

		AttackEngageFunction();
	}

	void Unit::ChaseEngage()
	{
		currentOrder = UnitState::Chase;

		ChaseEngageFunction();
	}

	void Unit::DeathEngage()
	{
		currentOrder = UnitState::Death;

		deathFunctionElapsed = 0.0f;

		DeathEngageFunction();
	}
#pragma endregion


#pragma region State Update()
	void Unit::IdleUpdate()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 0, 0 });

		idleElapsed += Time::GetDeltaTime();

		IdleUpdateFunction();
		// 데미지를 입으면 공격한 상대의 정보를 list에 등록하고 쫓아가기
	}

	void Unit::MoveUpdate()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 1, 0, 0 });

		moveFunctionElapsed += Time::GetDeltaTime();

		if (moveFunctionElapsed >= moveFunctionCallDelay)
			MoveUpdateFunction();
	}

	void Unit::AttackMoveUpdate()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0.4, 0.3, 0.25, 0 });

		moveFunctionElapsed += Time::GetDeltaTime();

		if (moveFunctionElapsed >= moveFunctionCallDelay)
			AttackMoveUpdateFunction();
	}

	void Unit::AttackUpdate()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 1, 0 });
		if (m_currentTargetObject->GetComponent<Unit>()->GetUnitCurrentState() == UnitState::Death)
		{
			m_opponentGameObjectList.remove(m_currentTargetObject);

			if (!m_opponentGameObjectList.empty())
				m_currentTargetObject = m_opponentGameObjectList.front();
		}
		attackFunctionElapsed += Time::GetDeltaTime();

		if (attackFunctionElapsed >= attackFunctionCallDelay)
			AttackUpdateFunction();
	}

	void Unit::ChaseUpdate()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0.5, 0.2, 0, 0 });
		chaseFunctionElapsed += Time::GetDeltaTime();

		if (chaseFunctionElapsed >= chaseFunctionCallDelay)
			ChaseUpdateFunction();
	}

	void Unit::DeathUpdate()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 0, 0 });
		deathFunctionElapsed += Time::GetDeltaTime();

		if (deathFunctionElapsed >= deathAnimationDelay)
			DeathUpdateFunction();
	}
#pragma endregion


#pragma region Engage Functions
	void Unit::IdleEngageFunction()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 0, 0 });

		bool isCurrentOpponentDeath = false;

		auto itr = m_opponentGameObjectList.begin();

		while (itr != m_opponentGameObjectList.end())
		{
			if ((*itr)->GetComponent<Unit>()->GetUnitCurrentState() == UnitState::Death)
			{
				itr = m_opponentGameObjectList.erase(itr);
				isCurrentOpponentDeath = true;
				break;
			}
			else
				itr++;
		}

		FindClosestOpponent();

		idleToChaseDelay = 0.0f;

		if (unitFSM.previousState == UnitState::Attack && !isCurrentOpponentDeath)
			idleToChaseDelay = 1.0f;

		if (unitFSM.previousState == UnitState::AttackMove)
			isAttackMoving = false;

		StopPosition();
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
		StopPosition();

		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 1, 0 });
	}

	void Unit::DeathEngageFunction()
	{
		StopPosition();

		GetGameObject()->GetComponent<NavigationAgent>()->SetRadius(0.0f);
		GetGameObject()->GetComponent<NavigationAgent>()->SetActive(false);
		//GetGameObject()->GetTransform()->SetWorldPosition(Vector3d(7, 0, 0));
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 1, 1, 0 });
	}
#pragma endregion

#pragma region Update Functions
	void Unit::IdleUpdateFunction()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 0, 0 });
	}

	void Unit::MoveUpdateFunction()
	{
		moveFunctionElapsed = 0.0f;

		GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(m_currentMovePosition);
	}

	void Unit::AttackMoveUpdateFunction()
	{
		moveFunctionElapsed = 0.0f;

		GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(m_currentMovePosition);
	}

	void Unit::ChaseUpdateFunction()
	{
		chaseFunctionElapsed = 0.0f;

		FindClosestOpponent();

		GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(m_currentTargetObject->GetTransform()->GetWorldPosition());
	}

	void Unit::AttackUpdateFunction()
	{
		attackFunctionElapsed = 0.0f;

		//if (m_currentTargetObject->GetComponent<Unit>()->GetUnitCurrentState() == UnitState::Death)


		ProjectileSystem::GetInstance()->Shoot(this, m_currentTargetObject->GetComponent<Unit>(), m_bulletSpeed);
	}

	void Unit::DeathUpdateFunction()
	{
		deathFunctionElapsed = 0.0f;

		GetGameObject()->SetSelfActive(false);
	}

#pragma endregion

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

void Unit::SetUnitHp(int p_Hp)
{
	m_hp = p_Hp;
}

void Unit::SetUnitAp(int p_Ap)
{
	m_ap = p_Ap;
}

void Unit::SetIdRadius(float radius)
{
	m_idDistance = radius;
}

void Unit::SetAtkRadius(float radius)
{
	m_atkDistance = radius;
}

void Unit::SetUnitSpeed(float speed)
{
	m_speed = speed;
}

Unit::UnitState Unit::GetUnitCurrentState() const
{
	return currentOrder;
}

int Unit::GetUnitAp() const
{
	return m_ap;
}

void Unit::Damaged(GameObject* opponentObject, int opponentAp)
{
	bool isAlreadyExist = false;
	for (auto itr = m_opponentGameObjectList.begin(); itr != m_opponentGameObjectList.end(); itr++)
	{
		if ((*itr) == opponentObject)
			isAlreadyExist = true;
	}

	if (!isAlreadyExist)
		m_opponentGameObjectList.push_back(opponentObject);

	if (m_currentTargetObject == nullptr)
		m_currentTargetObject = opponentObject;
	
	m_hp -= opponentAp;
}

void Unit::FindClosestOpponent()
{
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
}

void Unit::OrderMove(Vector3d position)
{
	m_currentMovePosition = position;
	currentOrder = UnitState::Move;
}

void Unit::OrderAttackMove(Vector3d position)
{
	m_currentMovePosition = position;
	currentOrder = UnitState::AttackMove;
	isAttackMoving = true;
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

