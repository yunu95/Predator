#include "Unit.h"
#include "ProjectileSystem.h"
#include "PlayerController.h"
#include "InputManager.h"

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
		[this]() { return (m_currentTargetObject != nullptr && idleElapsed >= idleToChaseDelay) && m_currentTargetObject->GetComponent<Unit>()->currentOrder != UnitState::Death; } });

	unitFSM.transitions[UnitState::Move].push_back({ UnitState::Idle,
		[this]() { return (GetGameObject()->GetTransform()->GetWorldPosition() - m_currentMovePosition).Magnitude() < 0.3f; } });

	unitFSM.transitions[UnitState::Move].push_back({ UnitState::AttackMove,
		[this]() { return currentOrder == UnitState::AttackMove; } });

	unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Idle,
		[this]() { return (GetGameObject()->GetTransform()->GetWorldPosition() - m_currentMovePosition).Magnitude() < 0.3f; } });

	unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Move,
		[this]() { return currentOrder == UnitState::Move; } });

	unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Chase,
		[this]() { return m_currentTargetObject != nullptr; } });

	unitFSM.transitions[UnitState::Chase].push_back({ UnitState::Idle,
		[this]() { return m_currentTargetObject == nullptr; } });

	unitFSM.transitions[UnitState::Chase].push_back({ UnitState::Attack,
		[this](){ return (GetGameObject()->GetTransform()->GetWorldPosition() - m_currentTargetObject->GetTransform()->GetWorldPosition()).Magnitude() <= m_atkDistance + 0.4f; } });

	unitFSM.transitions[UnitState::Attack].push_back({ UnitState::Idle,
		[this]() { return m_currentTargetObject == nullptr; } });

	unitFSM.transitions[UnitState::Attack].push_back({ UnitState::Move,
		[this]() { return currentOrder == UnitState::Move; } });

	unitFSM.transitions[UnitState::QSkill].push_back({ UnitState::Idle,
		[this]() { return currentOrder == UnitState::Idle; } });

	for (int i = static_cast<int>(UnitState::Idle); i < static_cast<int>(UnitState::QSkill); i++)
	{
		unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::QSkill,
		[this]() { return currentOrder == UnitState::QSkill; } });
	}

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
	unitFSM.engageAction[UnitState::QSkill] = [this]() { QSkillEngage(); };
	unitFSM.engageAction[UnitState::Death] = [this]() { DeathEngage(); };

	unitFSM.updateAction[UnitState::Idle] = [this]() { IdleUpdate(); };
	unitFSM.updateAction[UnitState::Move] = [this]() { MoveUpdate(); };
	unitFSM.updateAction[UnitState::AttackMove] = [this]() { AttackMoveUpdate(); };
	unitFSM.updateAction[UnitState::Chase] = [this]() { ChaseUpdate(); };
	unitFSM.updateAction[UnitState::Attack] = [this]() { AttackUpdate(); };
	unitFSM.updateAction[UnitState::QSkill] = [this]() { QSkillUpdate(); };
	unitFSM.updateAction[UnitState::Death] = [this]() { DeathUpdate(); };
}

void Unit::Update()
{
	unitFSM.UpdateState();
}

Unit::UnitType Unit::GetUnitType() const
{
	return m_unitType;
}

Unit::UnitSide Unit::GetUnitSide() const
{
	return m_unitSide;
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

	void Unit::QSkillEngage()
	{
		currentOrder = UnitState::QSkill;
		qSkillFunctionStartElapsed = 0.0f;
		qSkillFunctionStartedElapsed = 0.0f;

		StopMove();
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

		attackFunctionElapsed += Time::GetDeltaTime();

		if (attackFunctionElapsed >= attackFunctionCallDelay)
			AttackUpdateFunction();
	}

	void Unit::QSkillUpdate()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 0, 0 });

		qSkillFunctionStartElapsed += Time::GetDeltaTime();

		if (isSkillStarted)
		{
			qSkillFunctionStartedElapsed += Time::GetDeltaTime();
			if (qSkillFunctionStartedElapsed >= qSkillAnimationDuration)
			{
				isSkillStarted = false;
				currentOrder = UnitState::Idle;
				// 여기서 leftClickFunction을 스킬 사용 못하게 해야 한다....
				PlayerController::GetInstance()->SetLeftClickMove();
			}
		}

		if (qSkillFunctionStartElapsed >= qSkillStartDelay)
			QSkillUpdateFunction();
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
	/// <summary>
	/// 주요 기능 : m_currentTargetObject가 nullptr인 상태로 들어오면 상황에 따라 재정의.
	/// </summary>
	void Unit::IdleEngageFunction()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 0, 0 });

		DetermineCurrentTargetObject();

		idleToChaseDelay = 0.0f;

		if (unitFSM.previousState == UnitState::Attack && m_currentTargetObject == nullptr)
			idleToChaseDelay = 1.0f;

		if (unitFSM.previousState == UnitState::AttackMove)
			isAttackMoving = false;

		StopMove();
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
		StopMove();

		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 1, 0 });
	}

	void Unit::DeathEngageFunction()
	{
		m_opponentObjectList.clear();

		ReportUnitDeath();

		StopMove();

		GetGameObject()->GetComponent<NavigationAgent>()->SetRadius(0.0f);
		GetGameObject()->GetComponent<NavigationAgent>()->SetActive(false);
		GetGameObject()->GetTransform()->SetWorldPosition(Vector3d(99, 990, 990));
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

		DetermineCurrentTargetObject();

		GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(m_currentTargetObject->GetTransform()->GetWorldPosition());
	}

	void Unit::AttackUpdateFunction()
	{
		attackFunctionElapsed = 0.0f;

		//if (m_currentTargetObject->GetComponent<Unit>()->GetUnitCurrentState() == UnitState::Death)

		if(m_attackType == AttackType::Melee)

		ProjectileSystem::GetInstance()->Shoot(this, m_currentTargetObject->GetComponent<Unit>(), m_bulletSpeed);
	}

	void Unit::QSkillUpdateFunction()
	{
		qSkillFunctionStartElapsed = 0.0f;
		isSkillStarted = true;
		
		// 스킬 오브젝트 생성 (실제 스킬 기능)

	}

	void Unit::DeathUpdateFunction()
	{
		deathFunctionElapsed = 0.0f;

		//GetGameObject()->SetSelfActive(false);
	}

#pragma endregion

void Unit::StopMove()
{
	GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(GetGameObject()->GetTransform()->GetWorldPosition());
}

void Unit::SetUnitType(UnitType type)
{
	m_unitType = type;
}

void Unit::SetUnitSide(UnitSide side)
{
	m_unitSide = side;
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

int Unit::GetPlayerSerialNumber() const
{
	return playerSerialNumber;
}

void Unit::SetCurrentOrderMove()
{
	currentOrder = UnitState::Move;
}

void Unit::SetCurrentOrderAttackMove()
{
	currentOrder = UnitState::AttackMove;
}

int Unit::GetUnitAp() const
{
	return m_ap;
}

void Unit::Damaged(GameObject* opponentObject, int opponentAp)
{
	if (currentOrder == UnitState::Idle)
		AddToOpponentObjectList(opponentObject);

	m_hp -= opponentAp;
}

void Unit::DetermineCurrentTargetObject()
{
	bool isDistanceComparingStarted = false;

	float tempShortestDistance = 0.0f;

	for (auto e : m_opponentObjectList)
	{
		float distance = (GetGameObject()->GetTransform()->GetWorldPosition() - e->GetTransform()->GetWorldPosition()).Magnitude();

		if ((!isDistanceComparingStarted || tempShortestDistance > distance) && e->GetComponent<Unit>()->currentOrder != UnitState::Death)
		{
			tempShortestDistance = distance;
			m_currentTargetObject = e;
			isDistanceComparingStarted = true;
		}
	}
}

void Unit::ReportUnitDeath()
{
	for (auto e : m_recognizedThisList)
	{
		// 죽은 유닛이 아닌 죽은 유닛을 list에 갖고 있는 유닛의 함수 호출
		e->IdentifiedOpponentDeath(this->GetGameObject());
	}
}

void Unit::IdentifiedOpponentDeath(yunutyEngine::GameObject* diedOpponent)
{
	/// 죽은 유닛이 현재 타겟으로 지정한 유닛이라면
	if (m_currentTargetObject == diedOpponent)
		m_currentTargetObject = nullptr;

	/// 적군을 담고 있는 list에서 죽은 오브젝트 유닛을 빼준다.
	m_opponentObjectList.remove(diedOpponent);
}

void Unit::SetPlayerSerialNumber()
{
	static int localSerialNumber = 1;
	playerSerialNumber = localSerialNumber;
	localSerialNumber++;
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

	PlayerController::GetInstance()->SetLeftClickMove();
	// 다음 클릭은 Move로 바꿀 수 있도록 function 재정의.
		
	isAttackMoving = true;
}

void Unit::OrderQSkill(Vector3d position)
{
	currentOrder = UnitState::QSkill;

	PlayerController::GetInstance()->SetLeftClickMove();

	m_currentSkillPosition = position;
}

void Unit::AddToOpponentObjectList(yunutyEngine::GameObject* opponent)
{
	if (opponent->GetComponent<Unit>()->currentOrder != UnitState::Death && this->currentOrder != UnitState::Death)
	{
		m_opponentObjectList.push_back(opponent);

		if (m_currentTargetObject == nullptr)
			m_currentTargetObject = opponent;

		opponent->GetComponent<Unit>()->m_recognizedThisList.push_back(this);
	}
}

void Unit::DeleteFromOpponentObjectList(yunutyEngine::GameObject* obj)
{
	m_opponentObjectList.remove(obj);

	if (m_currentTargetObject == obj)
		m_currentTargetObject = nullptr;

	obj->GetComponent<Unit>()->m_recognizedThisList.remove(this);
}

