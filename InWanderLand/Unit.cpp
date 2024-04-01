#include "InWanderLand.h"
#include "Unit.h"
#include "PlayerController.h"
#include "AttackSystem.h"
#include "InputManager.h"
#include "PlayerSkillSystem.h"
#include "BossSkillSystem.h"
#include "Dotween.h"
#include "TacticModeSystem.h"
#include "IAnimation.h"
#include "UnitObjectPool.h"

void Unit::Start()
{
	m_initialAutoAttackDamage = m_autoAttackDamage;
	m_bulletSpeed = 5.1f;
	chaseUpdateDelay = 0.1f;
	m_currentHealthPoint = m_maxHealthPoint;

	dotween = GetGameObject()->GetComponent<Dotween>();
	m_navAgentComponent = GetGameObject()->GetComponent<NavigationAgent>();
	m_animatorComponent = GetGameObject()->GetComponent<yunutyEngine::graphics::Animator>();

	//returnToPoolFunction = []() {};
	unitFSM.transitions[UnitState::Idle].push_back({ UnitState::Move,
		[this]() { return currentOrder == UnitState::Move; } });

	unitFSM.transitions[UnitState::Idle].push_back({ UnitState::AttackMove,
		[this]() { return currentOrder == UnitState::AttackMove || (unitFSM.previousState == UnitState::Attack && isAttackMoving); } });

	unitFSM.transitions[UnitState::Idle].push_back({ UnitState::Chase,
		[this]() { return (m_currentTargetUnit != nullptr && idleElapsed >= idleToChaseDelay) && m_currentTargetUnit->currentOrder != UnitState::Death && m_idDistance > 0.1f && m_atkDistance > 0.1f; } });

	unitFSM.transitions[UnitState::Move].push_back({ UnitState::Idle,
		[this]() { return (GetGameObject()->GetTransform()->GetWorldPosition() - m_currentMovePosition).Magnitude() < 0.3f; } });

	unitFSM.transitions[UnitState::Move].push_back({ UnitState::AttackMove,
		[this]() { return currentOrder == UnitState::AttackMove; } });

	unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Idle,
		[this]() { return (GetGameObject()->GetTransform()->GetWorldPosition() - m_currentMovePosition).Magnitude() < 0.3f; } });

	unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Move,
		[this]() { return currentOrder == UnitState::Move; } });

	unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Chase,
		[this]() { return m_currentTargetUnit != nullptr; } });

	unitFSM.transitions[UnitState::Chase].push_back({ UnitState::Idle,
		[this]() { return m_currentTargetUnit == nullptr; } });

	unitFSM.transitions[UnitState::Chase].push_back({ UnitState::Move,
		[this]() { return currentOrder == UnitState::Move; } });

	unitFSM.transitions[UnitState::Chase].push_back({ UnitState::Attack,
		[this]() { return (GetGameObject()->GetTransform()->GetWorldPosition()
			- m_currentTargetUnit->GetTransform()->GetWorldPosition()).Magnitude() <= m_atkDistance + 0.4f; } });

	unitFSM.transitions[UnitState::Attack].push_back({ UnitState::Idle,
		[this]()
		{
			return m_currentTargetUnit == nullptr || 
				(((GetGameObject()->GetTransform()->GetWorldPosition() - m_currentTargetUnit->GetTransform()->GetWorldPosition()).Magnitude() > m_atkDistance + 0.4f)
					&& m_currentTargetUnit != tauntingThisUnit
					|| currentOrder == UnitState::Idle);
		} });

	unitFSM.transitions[UnitState::Attack].push_back({ UnitState::Move,
		[this]() { return currentOrder == UnitState::Move; } });

	for (int i = static_cast<int>(UnitState::Move); i < static_cast<int>(UnitState::Death); i++)
	{
		unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Idle,
		[this]() { return currentOrder == UnitState::Idle; } });
	}

	unitFSM.transitions[UnitState::Skill].push_back({ UnitState::Idle,
		[this]() { return currentOrder == UnitState::Idle; } });

	for (int i = static_cast<int>(UnitState::Idle); i < static_cast<int>(UnitState::Skill); i++)
	{
		unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Skill,
		[this]() { return currentOrder == UnitState::Skill || (TacticModeSystem::SingleInstance().IsTacticModeActivated(this)
			&& TacticModeSystem::SingleInstance().isTacticModeStarted); } });
	}

	for (int i = static_cast<int>(UnitState::Idle); i < static_cast<int>(UnitState::Paralysis); i++)
	{
		unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Paralysis,
		[this]() { return currentOrder == UnitState::Paralysis; } });
	}

	unitFSM.transitions[static_cast<UnitState>(UnitState::Paralysis)].push_back({ UnitState::Idle,
		[this]() { return currentOrder == UnitState::Idle; } });

	for (int i = static_cast<int>(UnitState::Idle); i < static_cast<int>(UnitState::Death); i++)
	{
		unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Death,
		[this]() { return m_currentHealthPoint <= 0; } });
	}

	unitFSM.engageAction[UnitState::Idle] = [this]() { IdleEngage(); };
	unitFSM.engageAction[UnitState::Move] = [this]() { MoveEngage(); };
	unitFSM.engageAction[UnitState::AttackMove] = [this]() { AttackMoveEngage(); };
	unitFSM.engageAction[UnitState::Chase] = [this]() { ChaseEngage(); };
	unitFSM.engageAction[UnitState::Attack] = [this]() { AttackEngage(); };
	unitFSM.engageAction[UnitState::Skill] = [this]() { SkillEngage(); };
	unitFSM.engageAction[UnitState::Paralysis] = [this]() { ParalysisEngage(); };
	unitFSM.engageAction[UnitState::Death] = [this]() { DeathEngage(); };

	unitFSM.updateAction[UnitState::Idle] = [this]() { IdleUpdate(); };
	unitFSM.updateAction[UnitState::Move] = [this]() { MoveUpdate(); };
	unitFSM.updateAction[UnitState::AttackMove] = [this]() { AttackMoveUpdate(); };
	unitFSM.updateAction[UnitState::Chase] = [this]() { ChaseUpdate(); };
	unitFSM.updateAction[UnitState::Attack] = [this]() { AttackUpdate(); };
	unitFSM.updateAction[UnitState::Skill] = [this]() { SkillUpdate(); };
	unitFSM.updateAction[UnitState::Death] = [this]() { DeathUpdate(); };

	m_animatorComponent->GetGI().Play(unitAnimations.m_idleAnimation);
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
	m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::white());
	m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_idleAnimation, animationLerpDuration, animationTransitionSpeed);

	DetermineCurrentTargetObject();

	idleToChaseDelay = 0.0f;

	if (unitFSM.previousState == UnitState::Attack && m_currentTargetUnit != nullptr && !isJustHitByQSkill)
		idleToChaseDelay = 1.0f;

	if (unitFSM.previousState == UnitState::AttackMove)
		isAttackMoving = false;

	StopMove();
}

void Unit::MoveEngage()
{
	currentOrder = UnitState::Move;

	moveFunctionElapsed = 0.0f;

	Vector3d mouseXZVector = Vector3d(m_currentMovePosition.x, 0, m_currentMovePosition.z);

	//dotween->DOLookAt(mouseXZVector, rotationTime, false);

	m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::blue());

	m_navAgentComponent->SetSpeed(m_speed);

	m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_walkAnimation, animationLerpDuration, animationTransitionSpeed);
}

void Unit::AttackMoveEngage()
{
	currentOrder = UnitState::AttackMove;

	moveFunctionElapsed = 0.0f;

	m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::gray());

	m_navAgentComponent->SetSpeed(m_speed);

	m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_walkAnimation, animationLerpDuration, animationTransitionSpeed);
}

void Unit::AttackEngage()
{
	currentOrder = UnitState::Attack;

	m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::red());

	//m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_idleAnimation, animationLerpDuration, animationTransitionSpeed);

	attackFunctionElapsed = 0.0f;
	attackAnimationFrameCheckNumber = 0;
	isAttackStarted = false;
	RotateUnit(m_currentTargetUnit->GetTransform()->GetWorldPosition());
	//dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotationTime, false);
	CheckCurrentAnimation(unitAnimations.m_idleAnimation);


	StopMove();
}

void Unit::ChaseEngage()
{
	currentOrder = UnitState::Chase;

	m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::green());

	dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotationTime, false);

	m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_walkAnimation, animationLerpDuration, animationTransitionSpeed);
}

void Unit::SkillEngage()
{
	currentOrder = UnitState::Skill;
	qSkillFunctionStartElapsed = 0.0f;

	/// 전술모드 동작 여부를 확인한다
	if (TacticModeSystem::SingleInstance().IsTacticModeActivated(this))
	{
		TacticModeSystem::SingleInstance().CallQueueFunction(this);
	}

	if (this->m_unitType == UnitType::Boss)
	{
		GetGameObject()->GetComponent<BossSkillSystem>()->ActivateSkillRandomly();
	}
	else
	{
		GetGameObject()->GetComponent<PlayerSkillSystem>()->ActivateSkill(m_currentSelectedSkill, m_currentSkillPosition);
	}

	StopMove();
}

void Unit::ParalysisEngage()
{
	m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_paralysisAnimation, animationLerpDuration, animationTransitionSpeed);
}

void Unit::DeathEngage()
{
	currentOrder = UnitState::Death;

	deathFunctionElapsed = 0.0f;

	m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_deathAnimation, animationLerpDuration, animationTransitionSpeed);
	m_opponentObjectSet.clear();

	ReportUnitDeath();

	StopMove();
}
#pragma endregion


#pragma region State Update()
void Unit::IdleUpdate()
{
	CheckCurrentAnimation(unitAnimations.m_idleAnimation);

	idleElapsed += Time::GetDeltaTime();

	if (idleElapsed >= 3.0f)
		DetermineCurrentTargetObject();

	// 데미지를 입으면 공격한 상대의 정보를 list에 등록하고 쫓아가기
}

void Unit::MoveUpdate()
{
	CheckCurrentAnimation(unitAnimations.m_walkAnimation);

	moveFunctionElapsed += Time::GetDeltaTime();

	//Vector3d mouseXZVector = Vector3d(m_currentMovePosition.x, 0, m_currentMovePosition.z);

	//LookAt(mouseXZVector);

	if (moveFunctionElapsed >= moveFunctionCallDelay)
	{
		moveFunctionElapsed = 0.0f;
		//RotateUnit(m_currentMovePosition);
		m_navAgentComponent->MoveTo(m_currentMovePosition);
	}
}

void Unit::AttackMoveUpdate()
{
	CheckCurrentAnimation(unitAnimations.m_walkAnimation);

	moveFunctionElapsed += Time::GetDeltaTime();

	Vector3d mouseXZVector = Vector3d(m_currentMovePosition.x, 0, m_currentMovePosition.z);

	//LookAt(mouseXZVector);

	if (moveFunctionElapsed >= moveFunctionCallDelay)
	{
		moveFunctionElapsed = 0.0f;
		//RotateUnit(m_currentMovePosition);
		m_navAgentComponent->MoveTo(m_currentMovePosition);
	}
}

void Unit::AttackUpdate()
{
	/// AttackState에서는 공격 후 다음 공격까지 남은 시간 동안 idleAnimation을 적용하므로 다른 방식으로 체크하는게 좋겠다.
	//CheckCurrentAnimation(m_attackAnimation);

	attackFunctionElapsed += Time::GetDeltaTime();
	//RotateUnit(m_currentTargetUnit->GetTransform()->GetWorldPosition());

	//LookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition());

	if (isAttackAnimationOperating)
	{
		attackAnimationFrameCheckNumber++;

		if (attackAnimationFrameCheckNumber >= unitAnimations.m_attackAnimation->GetTotalFrame())
		{
			m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_idleAnimation, animationLerpDuration, animationTransitionSpeed);
			attackAnimationFrameCheckNumber = 0;
			isAttackAnimationOperating = false;
			//dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotationTime, false);
		}
	}

	if (attackFunctionElapsed >= attackFunctionCallDelay || !isAttackStarted)
	{
		isAttackStarted = true;
		isAttackAnimationOperating = true;
		attackFunctionElapsed = 0.0f;
		m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_attackAnimation, animationLerpDuration, animationTransitionSpeed);
		RotateUnit(m_currentTargetUnit->GetTransform()->GetWorldPosition());
		GetGameObject()->GetComponent<AttackSystem>()->Attack(m_currentTargetUnit);
		DetermineCurrentTargetObject();
		CheckCurrentAnimation(unitAnimations.m_attackAnimation);
	}
	else if (attackFunctionElapsed < attackFunctionCallDelay)
	{
		dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotationTime, false);
	}
}

void Unit::SkillUpdate()
{
	qSkillFunctionStartElapsed += Time::GetDeltaTime();
	if (qSkillFunctionStartElapsed >= qSkillAnimationDuration)
	{
		isSkillStarted = false;
		currentOrder = UnitState::Idle;
		if (this->m_unitSide == UnitSide::Player)
			PlayerController::SingleInstance().SetLeftClickMove();
	}
}

void Unit::ChaseUpdate()
{
	CheckCurrentAnimation(unitAnimations.m_walkAnimation);

	chaseFunctionElapsed += Time::GetDeltaTime();

	//LookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition());

	if (chaseFunctionElapsed >= chaseFunctionCallDelay)
	{
		chaseFunctionElapsed = 0.0f;

		DetermineCurrentTargetObject();

		dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotationTime, false);

		m_navAgentComponent->MoveTo(m_currentTargetUnit->GetTransform()->GetWorldPosition());
	}
}

void Unit::DeathUpdate()
{
	CheckCurrentAnimation(unitAnimations.m_deathAnimation);

	deathFunctionElapsed += Time::GetDeltaTime();

	if (deathFunctionElapsed >= deathAnimationDelay)
	{
		if (returnToPoolFunction != nullptr)
		{
			returnToPoolFunction();
			ResetUnitMembers();
		}
		deathFunctionElapsed = 0.0f;
		//m_navAgentComponent->SetRadius(0.0f);
		//m_navAgentComponent->SetActive(false);
		GetGameObject()->SetSelfActive(false);
		//GetGameObject()->GetTransform()->SetWorldPosition(Vector3d(1000, 1000, 1000));
	}
}
#pragma endregion

void Unit::CheckCurrentAnimation(yunuGI::IAnimation* currentStateAnimation)
{
	if (m_animatorComponent->GetGI().GetCurrentAnimation() != currentStateAnimation)
	{
		m_animatorComponent->GetGI().ChangeAnimation(currentStateAnimation, animationLerpDuration, animationTransitionSpeed);
	}
}

void Unit::StopMove()
{
	if (m_navAgentComponent != nullptr)
		m_navAgentComponent->MoveTo(GetGameObject()->GetTransform()->GetWorldPosition());
}

void Unit::SetUnitType(UnitType type)
{
	m_unitType = type;
}

void Unit::SetUnitSide(UnitSide side)
{
	m_unitSide = side;
}

void Unit::SetUnitMaxHp(int p_Hp)
{
	m_maxHealthPoint = p_Hp;
}

void Unit::SetUnitAp(int p_Ap)
{
	m_autoAttackDamage = p_Ap;
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

void Unit::SetAttackDelay(float p_delay)
{
	attackFunctionCallDelay = p_delay;
}

Unit::UnitType Unit::GetPlayerSerialNumber() const
{
	return playerSerialNumber;
}

Unit::UnitState Unit::GetCurrentUnitState() const
{
	return currentOrder;
}

SkillPreviewSystem::SkillPreviewMesh Unit::GetSkillPreviewType(SkillEnum p_currentSkillType) const
{
	switch (p_currentSkillType)
	{
		case Unit::SkillEnum::Q:
			return m_qSkillPreviewType;
			break;
		case Unit::SkillEnum::W:
			return m_wSkillPreviewType;
			break;
		default:
			break;
	}
}

std::string Unit::GetUnitFbxName() const
{
	return m_fbxName;
}

void Unit::SetCurrentOrderMove()
{
	currentOrder = UnitState::Move;
}

void Unit::SetCurrentOrderAttackMove()
{
	currentOrder = UnitState::AttackMove;
}

int Unit::GetUnitDamage() const
{
	return m_autoAttackDamage;
}

void Unit::Damaged(Unit* opponentUnit, float opponentDmg)
{
	AddToOpponentObjectList(opponentUnit);
	//DetermineHitDamage(opponentDmg);
	m_currentHealthPoint -= opponentDmg;
	// ui로 표시되는, 혹은 최종 남은 체력은 반올림할 것인가 혹은 내림할 것인가는 아래에 구현.
}

void Unit::Damaged(float dmg)
{
	//DetermineHitDamage(dmg);
	//m_healthPoint -= m_finalHitDamage;
	m_currentHealthPoint -= dmg;
}

void Unit::Heal(float healingPoint)
{
	// 최대 체력이면 x
	m_currentHealthPoint += healingPoint;
}

void Unit::IncreaseAttackPower(float p_attackPowerIncrease)
{
	/// 우선은 기본 공격력만 증가하는 로직입니다.
	if(m_autoAttackDamage + p_attackPowerIncrease <= m_maxAutoAttackDamage)
		m_autoAttackDamage += p_attackPowerIncrease;
	else
	{
		m_autoAttackDamage = m_maxAutoAttackDamage;
	}
}

void Unit::IncreaseAttackSpeed(float p_attackSpeedIncrease)
{
	if (m_minAttackDelay < attackFunctionCallDelay / p_attackSpeedIncrease)
	{
		attackFunctionCallDelay /= p_attackSpeedIncrease;
	}

	else
	{
		attackFunctionCallDelay = m_minAttackDelay;
	}
}

void Unit::SetUnitDamageToZero(bool p_bool)
{
	if (p_bool == true)
	{
		m_beforeBlindDamage = m_autoAttackDamage;
		m_autoAttackDamage = 0;
	}

	else
	{
		m_autoAttackDamage = m_beforeBlindDamage;
	}
}

void Unit::MultipleUnitSpeed(float p_mul)
{
	m_speed *= p_mul;
}

void Unit::ResetUnitMembers()
{
	m_currentHealthPoint = m_maxHealthPoint;
	unitFSM.currentState = UnitState::Idle;
	m_currentTargetUnit = nullptr;
	m_opponentObjectSet.clear();
	m_recognizedThisSet.clear();
	m_attackingThisUnitSet.clear();
}

float Unit::DetermineAttackDamage(float p_damage)
{
	m_finalAttackDamage = p_damage;

	std::srand(std::time(0));

	m_randcriticalHitNumber = std::rand() % 100 + 1;
	if (m_randcriticalHitNumber <= m_criticalHitProbability)
	{
		m_finalAttackDamage *= m_criticalHitMultiplier;
	}

	return m_finalAttackDamage;
}

void Unit::SetStaticMeshComponent(yunutyEngine::graphics::StaticMeshRenderer* p_stcMesh)
{
	m_staticMeshRenderer = p_stcMesh;
}

void Unit::ChangeCurrentOpponentUnitForced(Unit* p_unit)
{
	if (tauntingThisUnit == nullptr)
	{
		//m_currentTargetUnit = nullptr;
		tauntingThisUnit = p_unit;
	}

	DetermineCurrentTargetObject();
}

void Unit::DeleteTauntingUnit()
{
	tauntingThisUnit = nullptr;
	DetermineCurrentTargetObject();
}

void Unit::SetUnitStateToDeath()
{
	currentOrder = UnitState::Death;
}

void Unit::SetUnitStateToSkill()
{
	currentOrder = UnitState::Skill;
}

void Unit::DetermineHitDamage(float p_onceCalculatedDmg)
{
	m_finalHitDamage = (m_defensePoint / 10.0f) / (1 - m_criticalDamageDecreaseMultiplier) / (1 - m_dodgeProbability);
}

void Unit::RotateUnit(Vector3d endPosition)
{
	Vector3d startPosition = GetGameObject()->GetTransform()->GetWorldPosition();
	Vector3d objectFront = GetGameObject()->GetTransform()->GetWorldRotation().Forward();
	Vector3d distanceVec = endPosition - startPosition;

	double dot = Vector3d::Dot(objectFront, startPosition - endPosition);

	double angle;
	double sq;
	double finalAngle;
	double finalDegree;

	// 회전 방향 판정
	Vector3d axis = Vector3d::Cross(objectFront, distanceVec);

	angle = (objectFront.x * distanceVec.x + objectFront.z * distanceVec.z);
	sq = (sqrt(pow(objectFront.x, 2) + pow(objectFront.z, 2)) *
		sqrt(pow(distanceVec.x, 2) + pow(distanceVec.z, 2)));

	// 두 벡터의 각도가 180도 이상이면 180을, -180 이하 이라면 -180을 
	//finalAngle = acos( max( -1.0f, min(1.0f, angle / sq) ) );
	finalAngle = acos(std::clamp(angle / sq, -1.0, 1.0));			// c++17 된다면
	finalDegree = 57.2969f * (finalAngle);

	if (axis.y < 0)
		finalDegree *= -1;

	if (abs(finalDegree) > 0.05)
		GetGameObject()->GetTransform()->SetWorldRotation(Quaternion({ 0.0f, finalDegree, 0.0f }));
}

void Unit::DetermineCurrentTargetObject()
{
	if (tauntingThisUnit != nullptr)
	{
		m_currentTargetUnit = tauntingThisUnit;
	}
	else
	{
		if (m_opponentObjectSet.empty())
		{
			m_currentTargetUnit = nullptr;
		}
		else
		{
			Unit* previousTargetUnit{ nullptr };
			previousTargetUnit = m_currentTargetUnit;

			bool isDistanceComparingStarted = false;

			float tempShortestDistance = 0.0f;

			for (auto e : m_opponentObjectSet)
			{
				float distance = (GetGameObject()->GetTransform()->GetWorldPosition() - e->GetTransform()->GetWorldPosition()).Magnitude();

				if ((!isDistanceComparingStarted || tempShortestDistance > distance) 
					&& (e->m_attackingThisUnitSet.size() < e->m_maxAggroNumber || e->m_attackingThisUnitSet.find(this) != e->m_attackingThisUnitSet.end()))
				{
					tempShortestDistance = distance;
					m_currentTargetUnit = e;
					isDistanceComparingStarted = true;
				}
			}

			if (m_currentTargetUnit != nullptr)
			{
				// 어그로 최대 수 제한 때문에 현재 타겟이 없을 경우
				m_currentTargetUnit->m_attackingThisUnitSet.insert(this);
				//previousTargetUnit = m_currentTargetUnit;
			}
			if (previousTargetUnit != nullptr && previousTargetUnit != m_currentTargetUnit)
			{
				previousTargetUnit->m_attackingThisUnitSet.erase(this);
			}
		}
	}
}

void Unit::ReportUnitDeath()
{
	for (auto e : m_recognizedThisSet)
	{
		// 죽은 유닛이 아닌 죽은 유닛을 list에 갖고 있는 유닛의 함수 호출
		e->IdentifiedOpponentDeath(this);
	}
}

void Unit::IdentifiedOpponentDeath(Unit* p_unit)
{
	if (m_currentTargetUnit == p_unit)
	{
		m_currentTargetUnit = nullptr;
	}
	m_attackingThisUnitSet.erase(p_unit);

	/// 적군을 담고 있는 list에서 죽은 오브젝트 유닛을 빼준다.
	m_opponentObjectSet.erase(p_unit);
	DetermineCurrentTargetObject();

	//for (auto e : m_recognizedThisList)
	//{
	//	e->DetermineCurrentTargetObject();
	//}
}

void Unit::SetPlayerSerialNumber(UnitType serialNum)
{
	playerSerialNumber = serialNum;
}

void Unit::OrderMove(Vector3d position)
{
	m_currentMovePosition = position;
	currentOrder = UnitState::Move;
	dotween->DOLookAt(position, rotationTime, false);
}

void Unit::OrderAttackMove(Vector3d position)
{
	m_currentMovePosition = position;
	currentOrder = UnitState::AttackMove;
	dotween->DOLookAt(position, rotationTime, false);

	PlayerController::SingleInstance().SetLeftClickMove();
	// 다음 클릭은 Move로 바꿀 수 있도록 function 재정의.

	isAttackMoving = true;
}

void Unit::OrderSkill(SkillEnum p_skillNum, Vector3d position)
{
	currentOrder = UnitState::Skill;
	m_skillPosition = position;
	m_currentSelectedSkill = p_skillNum;
	dotween->DOLookAt(position, rotationTime, false);

	PlayerController::SingleInstance().SetLeftClickEmpty();

	m_currentSkillPosition = position;
}

void Unit::OrderSkill(SkillEnum p_skillNum)
{
	/// warrior 2nd active skill 처럼 캐릭터의 회전이 필요 없는 스킬
	currentOrder = UnitState::Skill;
	m_currentSelectedSkill = p_skillNum;

	PlayerController::SingleInstance().SetLeftClickEmpty();
}

void Unit::SetSkillDuration(float p_duration)
{
	qSkillAnimationDuration = p_duration;
}

void Unit::SetSkillPreviewType(SkillPreviewSystem::SkillPreviewMesh p_qskill, SkillPreviewSystem::SkillPreviewMesh p_wskill)
{
	m_qSkillPreviewType = p_qskill;
	m_wSkillPreviewType = p_wskill;
}

void Unit::SetMaxAggroNumber(int p_num)
{
	m_maxAggroNumber = p_num;
}

void Unit::SetFbxName(std::string p_string)
{
	m_fbxName = p_string;
}

/// <summary>
/// 1. p_unit을 this->m_opponentObjectSet에 저장.
/// 2. p_unit->recognizeSet에 this를 저장
/// </summary>
/// <param name="p_unit"></param>
void Unit::AddToOpponentObjectList(Unit* p_unit)
{
	m_opponentObjectSet.insert(p_unit);
	p_unit->m_recognizedThisSet.insert(this);
	this->DetermineCurrentTargetObject();
}

void Unit::DeleteFromOpponentObjectList(Unit* p_unit)
{
	m_opponentObjectSet.erase(p_unit);
	p_unit->m_recognizedThisSet.erase(this);
	p_unit->m_attackingThisUnitSet.erase(this);
	this->DetermineCurrentTargetObject();
}

void Unit::SetNavField(NavigationField* p_navField)
{
	m_unitNavField = p_navField;
}

NavigationField*  Unit::GetNavField() const
{
	return m_unitNavField;
}

void Unit::MakeUnitPushedState(bool p_isCrushed)
{
	isJustHitByQSkill = p_isCrushed;
}

void Unit::MakeUnitParalysisState()
{
	currentOrder = UnitState::Paralysis;
}

void Unit::MakeUnitStateIdle()
{
	currentOrder = UnitState::Idle;
}

bool Unit::GetJustCrushedState() const
{
	return isJustHitByQSkill;
}

bool Unit::IsUnitDead() const
{
	return (m_currentHealthPoint <= 0);
}

