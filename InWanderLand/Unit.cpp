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
#include "SkillPreviewSystem.h"
#include "GameManager.h"

void Unit::Start()
{
	m_initialAutoAttackDamage = m_autoAttackDamage;
	m_bulletSpeed = 5.1f;
	chaseUpdateDelay = 0.1f;
	m_currentHealthPoint = m_maxHealthPoint;

	std::function<bool()> trapClassifingFunction = [=]()
		{
			return (m_unitType == UnitType::SpikeTrap || m_unitType == UnitType::ChessTrap || m_unitType == UnitType::TriggeredTrap);
		};

	for (auto each : OnCreated)
	{
		each();
	}
	//GetTransform()->SetWorldPosition(startPosition);

	dotween = GetGameObject()->GetComponent<Dotween>();
	m_skillSystemComponent = GetGameObject()->GetComponent<SkillSystem>();
	//m_navAgentComponent = GetGameObject()->GetComponent<NavigationAgent>();
	m_animatorComponent = GetGameObject()->GetComponent<yunutyEngine::graphics::Animator>();

	//returnToPoolFunction = []() {};
	unitFSM.transitions[UnitState::Idle].push_back({ UnitState::Move,
		[this]() { return currentOrder == UnitState::Move; } });

	unitFSM.transitions[UnitState::Idle].push_back({ UnitState::AttackMove,
		[this]() { return currentOrder == UnitState::AttackMove || (unitFSM.previousState == UnitState::Attack && isAttackMoving); } });

	unitFSM.transitions[UnitState::Idle].push_back({ UnitState::Chase,
		[this]() { return (m_currentTargetUnit != nullptr && idleElapsed >= idleToChaseDelay) && m_currentTargetUnit->currentOrder != UnitState::Death && m_idDistance > 0.1f && m_atkDistance > 0.1f; } });

	unitFSM.transitions[UnitState::Move].push_back({ UnitState::Idle,
		[this]() { return abs(GetGameObject()->GetTransform()->GetWorldPosition().x - m_currentMovePosition.x) < 0.2f && abs(GetGameObject()->GetTransform()->GetWorldPosition().z - m_currentMovePosition.z) < 0.2f; } });

	unitFSM.transitions[UnitState::Move].push_back({ UnitState::AttackMove,
		[this]() { return currentOrder == UnitState::AttackMove; } });

	unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Idle,
		[this]() { return abs(GetGameObject()->GetTransform()->GetWorldPosition().x - m_currentMovePosition.x) < 0.2f && abs(GetGameObject()->GetTransform()->GetWorldPosition().z - m_currentMovePosition.z) < 0.2f; } });

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
		[=]()
		{
			return (m_currentTargetUnit == nullptr
					|| (((GetGameObject()->GetTransform()->GetWorldPosition() - m_currentTargetUnit->GetTransform()->GetWorldPosition()).Magnitude() > m_atkDistance + 0.4f)
					|| currentOrder == UnitState::Idle));
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

	unitFSM.transitions[static_cast<UnitState>(UnitState::Idle)].push_back({ UnitState::OffsetMove,
	[this]() { return !GameManager::Instance().IsBattleSystemOperating() && m_unitSide == UnitSide::Player && m_unitType != UnitType::Warrior; } });

	unitFSM.transitions[static_cast<UnitState>(UnitState::OffsetMove)].push_back({ UnitState::Idle,
	[this]() { return GameManager::Instance().IsBattleSystemOperating(); } });

	//unitFSM.transitions[static_cast<UnitState>(UnitState::Move)].push_back({ UnitState::WaveEngage,
	//[this]() { return ; } });

	//unitFSM.transitions[static_cast<UnitState>(UnitState::OffsetMove)].push_back({ UnitState::WaveEngage,
	//[this]() { return ; } });

	unitFSM.engageAction[UnitState::Idle] = [this]() { IdleEngage(); };
	unitFSM.engageAction[UnitState::Move] = [this]() { MoveEngage(); };
	unitFSM.engageAction[UnitState::OffsetMove] = [this]() { OffsetMoveEngage(); };
	unitFSM.engageAction[UnitState::AttackMove] = [this]() { AttackMoveEngage(); };
	unitFSM.engageAction[UnitState::Chase] = [this]() { ChaseEngage(); };
	unitFSM.engageAction[UnitState::Attack] = [this]() { AttackEngage(); };
	unitFSM.engageAction[UnitState::Skill] = [this]() { SkillEngage(); };
	unitFSM.engageAction[UnitState::Paralysis] = [this]() { ParalysisEngage(); };
	unitFSM.engageAction[UnitState::Death] = [this]() { DeathEngage(); };

	unitFSM.updateAction[UnitState::Idle] = [this]() { IdleUpdate(); };
	unitFSM.updateAction[UnitState::Move] = [this]() { MoveUpdate(); };
	unitFSM.updateAction[UnitState::OffsetMove] = [this]() { OffsetMoveUpdate(); };
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

		m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::blue());

		m_navAgentComponent->SetSpeed(m_speed);
		m_navAgentComponent->MoveTo(m_currentMovePosition);

		m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_walkAnimation, animationLerpDuration, animationTransitionSpeed);
	}

	void Unit::OffsetMoveEngage()
	{
		StopMove();
		currentOrder = UnitState::OffsetMove;
		m_followingTargetUnit = PlayerController::SingleInstance().GetPlayerMap().find(UnitType::Warrior)->second;
		isFollowing = false;
		currentOrder = UnitState::OffsetMove;
		moveFunctionElapsed = 0.0f;
		m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::green());
		m_navAgentComponent->SetSpeed(m_speed);
		m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_idleAnimation, animationLerpDuration, animationTransitionSpeed);
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

		m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_idleAnimation, animationLerpDuration, animationTransitionSpeed);

		attackFunctionElapsed = 0.0f;
		animationFrameCheckNumber = 0;
		//RotateUnit(m_currentTargetUnit->GetTransform()->GetWorldPosition());
		dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
		CheckCurrentAnimation(unitAnimations.m_idleAnimation);


		StopMove();
	}

	void Unit::ChaseEngage()
	{
		currentOrder = UnitState::Chase;

		m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::green());

		dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);

		m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_walkAnimation, animationLerpDuration, animationTransitionSpeed);
	}

	void Unit::SkillEngage()
	{
		currentOrder = UnitState::Skill;
		m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_idleAnimation, animationLerpDuration, animationTransitionSpeed);
		skillFunctionStartElapsed = 0.0f;
		animationFrameCheckNumber = 0;
		isAttackAnimationOperating = false;
		isSkillStarted = false;

		dotween->DOLookAt(m_currentSkillPosition, rotateTime, false);

		if (m_unitType == UnitType::Boss)
		{
			auto temp = GetGameObject()->GetComponent<BossSkillSystem>();
			temp->SelectSkillRandomly();
			m_currentSelectedSkill = temp->GetCurrentSelectedSkillNumber();
		}

		m_currentSelectedSkillEngageDelay = m_skillDurationMap.find(m_currentSelectedSkill)->second;
		m_currentSkillAnimation = m_skillAnimationMap.find(m_currentSelectedSkill)->second;
		m_selectedSkillTimingFrame = m_skillTimingFrameMap.find(m_currentSelectedSkill)->second;

		/// 전술모드 동작 여부를 확인한다
		if (TacticModeSystem::SingleInstance().IsTacticModeActivated(this))
		{
			TacticModeSystem::SingleInstance().CallQueueFunction(this);
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

		if (m_unitSide == UnitSide::Enemy)
		{
			GameManager::Instance().AddCombo();
		}

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

		if (moveFunctionElapsed >= moveFunctionCallDelay)
		{
			moveFunctionElapsed = 0.0f;
			//RotateUnit(m_currentMovePosition);
			dotween->DOLookAt(m_currentMovePosition, rotateTime, false);
			m_navAgentComponent->MoveTo(m_currentMovePosition);
		}
	}

	void Unit::OffsetMoveUpdate()
	{
		/// m_followingTargetUnit 과의 거리가 일정 수치 이상으로 벌어진다면 이동 함수 호출.
		/// 다시 거리가 좁혀지면 StopMove. (Idle로 상태 변경)
		float betweenUnitDistance = (m_followingTargetUnit->GetTransform()->GetWorldPosition() - GetTransform()->GetWorldPosition()).Magnitude();
		
		float distance = (m_currentMovePosition - GetTransform()->GetWorldPosition()).Magnitude();

		if (betweenUnitDistance >= m_followEngageDinstance)
		{
			m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_walkAnimation, animationLerpDuration, animationTransitionSpeed);
			dotween->DOLookAt(m_followingTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
			m_navAgentComponent->MoveTo(m_followingTargetUnit->GetTransform()->GetWorldPosition());
			isFollowing = true;
		}
		else if (betweenUnitDistance <= m_stopFollowDinstance)
		{
			isFollowing = false;
			StopMove();
			m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_idleAnimation, animationLerpDuration, animationTransitionSpeed);
		}
		else if (betweenUnitDistance <= m_followEngageDinstance && betweenUnitDistance >= m_stopFollowDinstance && isFollowing)
		{
			m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_walkAnimation, animationLerpDuration, animationTransitionSpeed);
			dotween->DOLookAt(m_followingTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
			m_navAgentComponent->MoveTo(m_followingTargetUnit->GetTransform()->GetWorldPosition());
			isFollowing = true;
		}
	}

	void Unit::AttackMoveUpdate()
	{
		CheckCurrentAnimation(unitAnimations.m_walkAnimation);

		moveFunctionElapsed += Time::GetDeltaTime();

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

		//RotateUnit(m_currentTargetUnit->GetTransform()->GetWorldPosition());

		//LookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition());

		if (isAttackAnimationOperating)
		{
			animationFrameCheckNumber++;

			if (animationFrameCheckNumber >= unitAnimations.m_attackAnimation->GetTotalFrame())
			{
				m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_idleAnimation, animationLerpDuration, animationTransitionSpeed);
				animationFrameCheckNumber = 0;
				isAttackAnimationOperating = false;
				dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
			}

			if (animationFrameCheckNumber >= attackTimingFrame && !isAttacked)
			{
				GetGameObject()->GetComponent<AttackSystem>()->Attack(m_currentTargetUnit, m_attackOffset);
				//animationFrameCheckNumber = 0;
				isAttacked = true;
			}
		}
		else
		{
			attackFunctionElapsed += Time::GetDeltaTime();

			if (attackFunctionElapsed >= attackFunctionCallDelay)
			{
				isAttackAnimationOperating = true;
				isAttacked = false;
				attackFunctionElapsed = 0.0f;
				m_animatorComponent->GetGI().ChangeAnimation(unitAnimations.m_attackAnimation, animationLerpDuration, animationTransitionSpeed);
				DetermineCurrentTargetObject();
				CheckCurrentAnimation(unitAnimations.m_attackAnimation);
			}
			else if (attackFunctionElapsed < attackFunctionCallDelay)
			{
				dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
			}
		}
	}

	void Unit::SkillUpdate()
	{
		if (isAttackAnimationOperating)
		{
			animationFrameCheckNumber++;

			/// 스킬 애니메이션이 끝날 때 스킬이 끝날 것인가?
			//if (animationFrameCheckNumber >= m_currentSkillAnimation->GetTotalFrame())		/// 훗날 스킬 애니메이션으로 교체해주기. Engage에서 find해서 정해주기.
			//{
			//	currentOrder = UnitState::Idle;
			//}

			if (animationFrameCheckNumber >= m_selectedSkillTimingFrame)							/// 마찬가지로 engage에서 정해주기
			{
				m_skillSystemComponent->ActivateSkill(m_currentSelectedSkill, m_currentSkillPosition);
				animationFrameCheckNumber = 0;
				isAttackAnimationOperating = false;
			}
		}
		else if (!isSkillStarted)
		{
			skillFunctionStartElapsed += Time::GetDeltaTime();

			if (skillFunctionStartElapsed >= m_currentSelectedSkillEngageDelay)
			{
				isSkillStarted = true;
				isAttackAnimationOperating = true;
				skillFunctionStartElapsed = 0.0f;
				m_animatorComponent->GetGI().ChangeAnimation(m_currentSkillAnimation, animationLerpDuration, animationTransitionSpeed);
			}
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

			dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);

			m_navAgentComponent->MoveTo(m_currentTargetUnit->GetTransform()->GetWorldPosition());
		}
	}

	void Unit::DeathUpdate()
	{
		CheckCurrentAnimation(unitAnimations.m_deathAnimation);

		deathFunctionElapsed += Time::GetDeltaTime();

		if (deathFunctionElapsed >= deathAnimationDelay)
			{
				for (auto each : OnDeath)
				{
					each();
				}

				if (returnToPoolFunction != nullptr)
				{
					returnToPoolFunction();
					//ResetUnitMembers();
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

void Unit::SetAttackTimingFrame(int p_frame)
{
	attackTimingFrame = p_frame;
}

Unit::UnitType Unit::GetPlayerSerialNumber() const
{
	return playerSerialNumber;
}

Unit::UnitState Unit::GetCurrentUnitState() const
{
	return currentOrder;
}

SkillPreviewMesh Unit::GetSkillPreviewType(SkillEnum p_currentSkillType) const
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

float Unit::GetAttackOffset() const
{
	return m_attackOffset;
}

int Unit::GetUnitDamage() const
{
	return m_autoAttackDamage;
}

void Unit::Damaged(Unit* opponentUnit, float opponentDmg)
{
	AddToOpponentObjectList(opponentUnit);
	DetermineHitDamage(opponentDmg);
	m_currentHealthPoint -= m_finalHitDamage;
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
		increasedAttackSpeed *= p_attackSpeedIncrease;
	}
	else
	{
		attackFunctionCallDelay = m_minAttackDelay;
		increasedAttackSpeed = attackFunctionCallDelay / m_minAttackDelay;
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
	m_currentTargetUnit = nullptr;
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
	m_finalHitDamage = p_onceCalculatedDmg - ((m_defensePoint / 10.0f) / (1 - m_criticalDamageDecreaseMultiplier)) / (1 - m_dodgeProbability);
}

void Unit::RotateUnit(Vector3d endPosition)
{
	Vector3d startPosition = GetGameObject()->GetTransform()->GetWorldPosition();
	Vector3d objectFront = GetGameObject()->GetTransform()->GetWorldRotation().Forward();
	Vector3d distanceVec = endPosition - startPosition;

	float distance = distanceVec.Magnitude();

	Vector3d directionVector = distanceVec.Normalized();

	Vector3d movedPositionPerFrame = GetGameObject()->GetTransform()->GetWorldPosition() + (directionVector * m_speed * Time::GetDeltaTime());

	Vector3d afterDirectionVector = (endPosition - movedPositionPerFrame).Normalized();

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
	
	if (!isnan(finalDegree))
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
	m_previousMovePosition = m_currentMovePosition;
	m_currentMovePosition = position;

	if (GameManager::Instance().IsBattleSystemOperating() || m_unitType == UnitType::Warrior)
	{
		currentOrder = UnitState::Move;
		dotween->DOLookAt(position, rotateTime, false);
	}

	//if (m_unitType != UnitType::Warrior && !GameManager::Instance().IsBattleSystemOperating())
	//{
	//	m_followingTargetUnit = PlayerController::SingleInstance().GetPlayerMap().find(UnitType::Warrior)->second;
	//	currentOrder = UnitState::OffsetMove;
	//}
	//else
	//{
	//	currentOrder = UnitState::Move;
	//	dotween->DOLookAt(position, rotateTime, false);
	//}
}

void Unit::OrderAttackMove(Vector3d position)
{
	m_currentMovePosition = position;
	currentOrder = UnitState::AttackMove;
	dotween->DOLookAt(position, rotateTime, false);

	PlayerController::SingleInstance().SetLeftClickMove();
	// 다음 클릭은 Move로 바꿀 수 있도록 function 재정의.

	isAttackMoving = true;
}

void Unit::OrderSkill(SkillEnum p_skillNum, Vector3d position)
{
	currentOrder = UnitState::Skill;
	m_skillPosition = position;
	m_currentSelectedSkill = p_skillNum;
	dotween->DOLookAt(position, rotateTime, false);

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

void Unit::RegisterSkillDuration(float p_duration)
{
	qSkillAnimationDuration = p_duration;
}

void Unit::RegisterSkillDuration(SkillEnum p_skillEnum, float p_duration)
{
	m_skillDurationMap.insert({ p_skillEnum, p_duration });
}

void Unit::RegisterSkillAnimation(SkillEnum p_skillEnum, yunuGI::IAnimation* p_anim)
{
	m_skillAnimationMap.insert({ p_skillEnum, p_anim });
}

void Unit::RegisterSkillTimingFrame(SkillEnum p_skillEnum, int p_frame)
{
	m_skillTimingFrameMap.insert({ p_skillEnum, p_frame });
}

void Unit::SetSkillPreviewType(SkillPreviewMesh p_qskill, SkillPreviewMesh p_wskill)
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

void Unit::SetAttackOffset(float p_offset)
{
	m_attackOffset = p_offset;
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

void Unit::SetUnitStateIdle()
{
	currentOrder = UnitState::Idle;

	if (m_unitSide == UnitSide::Player)
	{
		PlayerController::SingleInstance().SetLeftClickMove();
	}
}

bool Unit::GetJustCrushedState() const
{
	return isJustHitByQSkill;
}

bool Unit::IsUnitDead() const
{
	return (m_currentHealthPoint <= 0);
}

