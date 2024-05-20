#include "InWanderLand.h"
#include "Unit.h"
#include "PlayerController.h"
#include "AttackSystem.h"
#include "InputManager.h"
#include "PlayerSkillSystem.h"
#include "ChessTrapSkillSystem.h"
#include "SpikeSkillSystem.h"
#include "BossSkillSystem.h"
#include "Dotween.h"
#include "TacticModeSystem.h"
#include "IAnimation.h"
#include "SkillPreviewSystem.h"
#include "GameManager.h"
#include "BurnEffect.h"
#include "CursorDetector.h"
#include "DebuggingMeshPool.h"
#include "StatusEffect.h"
#include "CinematicManager.h"

void Unit::OnEnable()
{
    for (auto each : OnCreated)
    {
        each();
    }
    if (m_navAgentComponent)
    {
        m_navAgentComponent->GetGameObject()->SetSelfActive(true);
    }
}
void Unit::OnDisable()
{
    if (m_navAgentComponent)
    {
        m_navAgentComponent->GetGameObject()->SetSelfActive(false);
    }
}

void Unit::Start()
{
    for (int i = 0; i < UnitState::StateEnd; i++)
    {
        m_stateTimerVector.push_back(GetGameObject()->AddComponent<TimerComponent>());
    }

    switch (m_unitType)
    {
    case Unit::UnitType::Warrior:
    case Unit::UnitType::Magician:
    case Unit::UnitType::Healer:
        unitStatusUI = UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_Heroes));
        break;
    case UnitType::MeleeEnemy:
        unitStatusUI = UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_MeleeEnemy));
        break;
    }
    if (!unitStatusUI.expired())
    {
        unitStatusUI.lock()->GetTransform()->SetWorldPosition(UIManager::Instance().GetUIPosFromWorld(GetTransform()->GetWorldPosition()));
        unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_HP_Cells)->adjuster->SetTargetFloat(m_maxHealthPoint);
        unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_HP_Number_Max)->SetNumber(m_maxHealthPoint);
    }

    switch (m_unitType)
    {
    case Unit::UnitType::Warrior:
        unitStatusPortraitUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin);
        break;
    case Unit::UnitType::Magician:
        unitStatusPortraitUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula);
        break;
    case Unit::UnitType::Healer:
        unitStatusPortraitUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel);
        break;
    default:
        break;
    }

    m_initialAutoAttackDamage = m_autoAttackDamage;
    m_bulletSpeed = 5.1f;
    chaseUpdateDelay = 0.1f;
    SetUnitCurrentHp(m_maxHealthPoint);

    //m_currentSkillAnimation->ClearEvent();
    //unitAnimations.m_attackAnimation->ClearEvent();
    //GetTransform()->SetWorldPosition(startPosition);

    dotween = GetGameObject()->GetComponent<Dotween>();
    m_skillSystemComponent = GetGameObject()->GetComponent<SkillSystem>();
    m_burnEffect = GetGameObject()->GetComponent<BurnEffect>();
    m_animatorComponent = GetGameObject()->GetComponent<yunutyEngine::graphics::Animator>();

    if (m_navAgentComponent)
        m_navAgentComponent->SetSpeed(m_speed);

    /// Idle
    unitFSM.transitions[UnitState::Idle].push_back({ UnitState::Move,
        [this]() { return (currentOrder == UnitState::Move && !TacticModeSystem::Instance().IsUnitsPerformingCommand()) ||
        (currentOrder == UnitState::Move && TacticModeSystem::Instance().IsUnitsPerformingCommand()); } });

    unitFSM.transitions[UnitState::Idle].push_back({ UnitState::AttackMove,
        [this]() { return currentOrder == UnitState::AttackMove || (unitFSM.previousState == UnitState::Attack && isAttackMoving); } });

    unitFSM.transitions[UnitState::Idle].push_back({ UnitState::Chase,
        [this]() { return  (GameManager::Instance().IsWaveEngageMotionEnd() && (m_currentTargetUnit != nullptr &&
                            idleElapsed >= idleToChaseDelay) && m_currentTargetUnit->currentOrder != UnitState::Death &&
                            m_idDistance > 0.1f && m_atkDistance > 0.1f) && !TacticModeSystem::Instance().IsUnitsPerformingCommand(); } });

    /// Move
    unitFSM.transitions[UnitState::Move].push_back({ UnitState::Idle,
        [this]() { return currentOrder == UnitState::Idle; } });

    unitFSM.transitions[UnitState::Move].push_back({ UnitState::AttackMove,
        [this]() { return currentOrder == UnitState::AttackMove; } });

    unitFSM.transitions[UnitState::Move].push_back({ UnitState::WaveStart,
        [this]() { return GameManager::Instance().IsPlayerJustEnteredWaveRegion(); } });

    /// Chase
    unitFSM.transitions[UnitState::Chase].push_back({ UnitState::Idle,
    [this]() { return m_currentTargetUnit == nullptr; } });

    unitFSM.transitions[UnitState::Chase].push_back({ UnitState::Move,
        [this]() { return currentOrder == UnitState::Move; } });

    unitFSM.transitions[UnitState::Chase].push_back({ UnitState::Attack,
        [this]() { return (GetGameObject()->GetTransform()->GetWorldPosition()
            - m_currentTargetUnit->GetTransform()->GetWorldPosition()).Magnitude() <= m_atkDistance + 0.4f; } });

    /// Attack
    unitFSM.transitions[UnitState::Attack].push_back({ UnitState::Idle,
        [=]()
        {
            return (m_currentTargetUnit == nullptr/*m_currentTargetUnit->GetCurrentUnitState() == UnitState::Death*/
                    || (((GetGameObject()->GetTransform()->GetWorldPosition() - m_currentTargetUnit->GetTransform()->GetWorldPosition()).Magnitude() > m_atkDistance + 0.4f)
                    || currentOrder == UnitState::Idle));
        } });

    unitFSM.transitions[UnitState::Attack].push_back({ UnitState::Move,
        [this]() { return currentOrder == UnitState::Move; } });

    /// AttackMove
    unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Idle,
        [this]() { return abs(GetGameObject()->GetTransform()->GetWorldPosition().x - m_currentMovePosition.x) < 0.2f && abs(GetGameObject()->GetTransform()->GetWorldPosition().z - m_currentMovePosition.z) < 0.2f; } });

    unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Move,
        [this]() { return currentOrder == UnitState::Move; } });

    unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Chase,
        [this]() { return m_currentTargetUnit != nullptr || (TacticModeSystem::Instance().IsUnitsPerformingCommand() && isTacticAttackMovePermitted); } });

    /// Paralysis
    unitFSM.transitions[UnitState::Paralysis].push_back({ UnitState::Idle,
        [this]() { return currentOrder == UnitState::Idle; } });

    /// Death




    for (int i = static_cast<int>(UnitState::Move); i < static_cast<int>(UnitState::Death); i++)
    {
        unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Idle,
        [this]() { return currentOrder == UnitState::Idle; } });
    }

    for (int i = static_cast<int>(UnitState::Idle); i < static_cast<int>(UnitState::Paralysis); i++)
    {
        unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Paralysis,
        [this]() { return currentOrder == UnitState::Paralysis; } });
    }

    for (int i = static_cast<int>(UnitState::Idle); i < static_cast<int>(UnitState::Resurrect); i++)
    {
        unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Resurrect,
        [this]() { return m_currentHealthPoint <= 0 && m_resurrectingMaxCount >= m_currentResurrectingCount && !IsAllExtraPlayerUnitDead() && m_unitSide == UnitSide::Player; } });
    }

    for (int i = static_cast<int>(UnitState::Idle); i < static_cast<int>(UnitState::Death); i++)
    {
        unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Death,
        [this]() { return m_currentHealthPoint <= 0 && m_resurrectingMaxCount < m_currentResurrectingCount; } });
    }

    unitFSM.engageAction[UnitState::Idle] = [this]() { IdleEngage(); };
    unitFSM.engageAction[UnitState::Move] = [this]() { MoveEngage(); };
    unitFSM.engageAction[UnitState::AttackMove] = [this]() { AttackMoveEngage(); };
    unitFSM.engageAction[UnitState::Chase] = [this]() { ChaseEngage(); };
    unitFSM.engageAction[UnitState::Attack] = [this]() { AttackEngage(); };
    unitFSM.engageAction[UnitState::Paralysis] = [this]() { ParalysisEngage(); };
    unitFSM.engageAction[UnitState::Death] = [this]() { DeathEngage(); };

    unitFSM.updateAction[UnitState::Idle] = [this]() { IdleUpdate(); };
    unitFSM.updateAction[UnitState::Move] = [this]() { MoveUpdate(); };
    unitFSM.updateAction[UnitState::AttackMove] = [this]() { AttackMoveUpdate(); };
    unitFSM.updateAction[UnitState::Chase] = [this]() { ChaseUpdate(); };
    unitFSM.updateAction[UnitState::Attack] = [this]() { AttackUpdate(); };
    unitFSM.updateAction[UnitState::Death] = [this]() { DeathUpdate(); };


    if (unitAnimations.m_attackAnimation != nullptr)
    {
        AttackSystem* atkSys = GetGameObject()->GetComponent<AttackSystem>();

        m_animatorComponent->PushAnimationWithFunc(unitAnimations.m_attackAnimation, attackTimingFrame, [=]()
            {
                currentOrder = UnitState::Attack;

                if (m_currentTargetUnit != nullptr)
                {
                    atkSys->Attack(m_currentTargetUnit, m_attackOffset);

                    if (isPermittedToTacticAction)
                    {
                        isPermittedToTacticAction = false;
                        isTacticAttackMovePermitted = false;
                        //EnemyActionOnTacticModeEngaged();
                        TacticModeSystem::Instance().ReportTacticActionFinished();
                        currentOrder = UnitState::Idle;
                        /// 현재 공격타겟에 대한 처리를 해야 가만히 있을 듯.
                        m_currentTargetUnit = nullptr;
                        tauntingThisUnit = nullptr;

                        /// 임시 - 애니메이션을 공유하고 있어서 공격 애니메이션 스피드를 1로 돌려줘야 함 ㅠ
                        ResumeAnimation();
                        //unitAnimations.m_attackAnimation->SetPlaySpeed(1.0f);
                        //unitAnimations.m_walkAnimation->SetPlaySpeed(1.0f);
                    }
                }
            });

        m_animatorComponent->PushAnimationWithFunc(unitAnimations.m_attackAnimation, unitAnimations.m_attackAnimation->GetTotalFrame() - 1, [=]()
            {
                isAttackAnimationOperating = false;
            });
    }

    if (m_unitSide == UnitSide::Player)
    {
        m_playerSkillSystem = GetGameObject()->GetComponent<PlayerSkillSystem>();
    }

    if (m_animatorComponent)
        m_animatorComponent->Play(unitAnimations.m_idleAnimation);

    if (m_unitType == UnitType::ChessTrap)
    {
        int a = 0;
    }
}

void Unit::Update()
{
    if (m_unitType == UnitType::ChessTrap)
    {
        int a = 0;
    }
    if (m_navAgentComponent && isFollowingNavAgent)
        GetTransform()->SetWorldPosition(m_navAgentComponent->GetTransform()->GetWorldPosition());
    if (m_unitSide == UnitSide::Player)
        unitFSM.UpdateState();
    else
    {
        if (!TacticModeSystem::Instance().IsUnitsPerformingCommand())
        {
            if (!TacticModeSystem::Instance().IsOrderingTimingNow())
            {
                unitFSM.UpdateState();
            }
        }
    }
    if (!unitStatusUI.expired())
        unitStatusUI.lock()->GetTransform()->SetWorldPosition(UIManager::Instance().GetUIPosFromWorld(GetTransform()->GetWorldPosition()));
    auto camTrsform = graphics::Camera::GetMainCamera()->GetTransform();
    auto shouldBeScreenZero = camTrsform->GetWorldPosition() + camTrsform->GetWorldRotation().Forward() * 10.0f + camTrsform->GetWorldRotation().Right() * 9.6f;
    //unitStatusUI.lock()->GetTransform()->SetWorldPosition(UIManager::Instance().GetUIPosFromWorld(shouldBeScreenZero));
}

void Unit::OnDestroy()
{
    //if (m_unitSide == UnitSide::Player)
    //    PlayerController::Instance().ErasePlayerUnit(dynamic_cast<PlayerUnit*>(this));
    if (!unitStatusUI.expired())
        Scene::getCurrentScene()->DestroyGameObject(unitStatusUI.lock()->GetGameObject());
    if (m_navAgentComponent)
        Scene::getCurrentScene()->DestroyGameObject(m_navAgentComponent->GetGameObject());
}
void Unit::OnTransformUpdate()
{

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
	StopAllStateTimer();
    currentOrder = UnitState::Idle;
    m_currentTargetUnit = nullptr;
    idleElapsed = 0.0f;
    if (m_staticMeshRenderer != nullptr)
        m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::white());

    ChangeAnimation(unitAnimations.m_idleAnimation);

    if (!TacticModeSystem::Instance().IsUnitsPerformingCommand())
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
    StopAllStateTimer();
    currentOrder = UnitState::Move;

    m_currentTargetUnit = nullptr;
    m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::blue());
    m_navAgentComponent->SetSpeed(m_speed);
    m_navAgentComponent->MoveTo(m_currentMovePosition);
    dotween->DOLookAt(m_currentMovePosition, rotateTime, false);

    ChangeAnimation(unitAnimations.m_walkAnimation);

    m_stateTimerVector[UnitState::Move]->pushDuration = moveFunctionCallDelay;
    m_stateTimerVector[UnitState::Move]->m_isRepeated = true;
    m_stateTimerVector[UnitState::Move]->onCompleteFunction = [this]()
        {
			dotween->DOLookAt(m_currentMovePosition, rotateTime, false);
			m_navAgentComponent->MoveTo(m_currentMovePosition);
        };
    m_stateTimerVector[UnitState::Move]->ActivateTimer();
}

void Unit::AttackMoveEngage()
{
	StopAllStateTimer();
    currentOrder = UnitState::AttackMove;

    m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::gray());

    m_navAgentComponent->SetSpeed(m_speed);

    ChangeAnimation(unitAnimations.m_walkAnimation);

	m_stateTimerVector[UnitState::AttackMove]->pushDuration = moveFunctionCallDelay;
	m_stateTimerVector[UnitState::AttackMove]->m_isRepeated = true;
	m_stateTimerVector[UnitState::AttackMove]->onCompleteFunction = [this]()
		{
			dotween->DOLookAt(m_currentMovePosition, rotateTime, false);
			m_navAgentComponent->MoveTo(m_currentMovePosition);
		};
	m_stateTimerVector[UnitState::AttackMove]->ActivateTimer();
}

void Unit::AttackEngage()
{
	StopAllStateTimer();
    currentOrder = UnitState::Attack;

    m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::red());

    ChangeAnimation(unitAnimations.m_idleAnimation);

    AttackSystem* atkSys = GetGameObject()->GetComponent<AttackSystem>();
    isAttackAnimationOperating = false;
    isAnimationChangedAttackToIdle = false;

    dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
    CheckCurrentAnimation(unitAnimations.m_idleAnimation);

    StopMove();

	m_stateTimerVector[UnitState::Attack]->pushDuration = attackFunctionCallDelay;
	m_stateTimerVector[UnitState::Attack]->m_isRepeated = true;
	m_stateTimerVector[UnitState::Attack]->onUpdate = [this](float t)
		{
            if (!isAttackAnimationOperating)
            {
				if (!isAnimationChangedAttackToIdle)
				{
					ChangeAnimation(unitAnimations.m_idleAnimation);
					isAnimationChangedAttackToIdle = true;
				}
				dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
				CheckCurrentAnimation(unitAnimations.m_idleAnimation);
            }
		};
	m_stateTimerVector[UnitState::Attack]->onCompleteFunction = [this]()
		{
			DetermineCurrentTargetObject();
			isAttackAnimationOperating = true;
			isAnimationChangedAttackToIdle = false;
			ChangeAnimation(unitAnimations.m_attackAnimation);
			CheckCurrentAnimation(unitAnimations.m_attackAnimation);
		};
	m_stateTimerVector[UnitState::Attack]->ActivateTimer();
}

void Unit::ChaseEngage()
{
    StopAllStateTimer();

    currentOrder = UnitState::Chase;

    m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::green());

    dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);

    ChangeAnimation(unitAnimations.m_walkAnimation);

	m_navAgentComponent->MoveTo(m_currentTargetUnit->GetTransform()->GetWorldPosition());

    m_stateTimerVector[UnitState::Chase]->pushDuration = chaseFunctionCallDelay;
	m_stateTimerVector[UnitState::Chase]->m_isRepeated = true;
	m_stateTimerVector[UnitState::Chase]->onCompleteFunction = [this]()
		{
			DetermineCurrentTargetObject();
			dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
			m_navAgentComponent->MoveTo(m_currentTargetUnit->GetTransform()->GetWorldPosition());
		};
}

void Unit::ParalysisEngage()
{
	StopAllStateTimer();
    ChangeAnimation(unitAnimations.m_paralysisAnimation);
}

void Unit::DeathEngage()
{
    if (!unitStatusUI.expired())
        unitStatusUI.lock()->DisableElement();

	StopAllStateTimer();
    currentOrder = UnitState::Death;

    deathFunctionElapsed = 0.0f;

    //m_animatorComponent->ChangeAnimation(unitAnimations.m_deathAnimation, animationLerpDuration, animationTransitionSpeed);
    m_opponentObjectSet.clear();

    ReportUnitDeath();

    //if (m_cursorDetectorComponent)
    //	m_cursorDetectorComponent->EraseUnitFromContainer(this);

    if (m_unitSide == UnitSide::Enemy)
    {

        //StopAnimation();
        m_animatorComponent->Pause();
        m_burnEffect->DisAppear();
        GameManager::Instance().AddCombo();
        //deathEngageFunction();
    }
    else
    {
        ChangeAnimation(unitAnimations.m_deathAnimation);
    }

    StopMove();
}

//void Unit::WaveStartEngage()
//{
//    if (!unitStatusUI.expired())
//        unitStatusUI.lock()->EnableElement();
//    currentOrder = UnitState::WaveStart;
//    moveFunctionElapsed = 0.0f;
//    ChangeAnimation(unitAnimations.m_walkAnimation);
//
//    m_navAgentComponent->MoveTo(m_waveStartPosition);
//
//    //dotween->DOLookAt(m_waveStartPosition, rotateTime, false);
//
//    //m_navAgentComponent->SetActive(false);
//    //dotween->DOMove(m_currentMovePosition, 1.0f).OnComplete([this]()
//    //	{
//    //		m_navAgentComponent->SetActive(true);
//    //		m_navAgentComponent->AssignToNavigationField(m_unitNavField);
//    //		m_navAgentComponent->Relocate(m_currentMovePosition);
//    //		GameManager::Instance().ReportWaveStartStateEnd(this);
//    //	});
//}

//void Unit::WaveMotionEngage()
//{
//    currentOrder = UnitState::WaveMotion;
//
//    ChangeAnimation(unitAnimations.m_battleEngageAnimation);
//
//    /*	if (m_unitType != UnitType::Warrior)
//            dotween->DOLookAt(m_currentBelongingWavePosition, rotateTime, false);*/
//}
//
//void Unit::ResurrectEngage()
//{
//    currentOrder = UnitState::Resurrect;
//    deathFunctionElapsed = 0.0f;
//    ChangeAnimation(unitAnimations.m_deathAnimation);
//    m_opponentObjectSet.clear();
//    ReportUnitDeath();
//
//    GetGameObject()->GetComponent<physics::Collider>()->SetActive(false);
//
//    StopMove();
//}

#pragma endregion


#pragma region State Update()
void Unit::IdleUpdate()
{
    CheckCurrentAnimation(unitAnimations.m_idleAnimation);

    if (!IsTacticModeQueueEmpty() && !TacticModeSystem::Instance().IsOrderingTimingNow() && isPermittedToTacticAction)
    {
        m_tacticModeQueue.front()();
        m_tacticModeQueue.pop();
        ResumeAnimation();
    }

    idleElapsed += Time::GetDeltaTime() * m_localTimeScale;

    if (idleElapsed >= 3.0f)
    {
        if (!TacticModeSystem::Instance().IsUnitsPerformingCommand())
        {
            DetermineCurrentTargetObject();
        }
    }
    // 데미지를 입으면 공격한 상대의 정보를 list에 등록하고 쫓아가기
}

void Unit::MoveUpdate()
{
    CheckCurrentAnimation(unitAnimations.m_walkAnimation);

    if (abs(GetGameObject()->GetTransform()->GetWorldPosition().x - m_currentMovePosition.x) < 0.2f &&
        abs(GetGameObject()->GetTransform()->GetWorldPosition().z - m_currentMovePosition.z) < 0.2f)
    {
        currentOrder = UnitState::Idle;
        dotween->StopAllDotweenFunction();
        if (isPermittedToTacticAction)
        {
            isPermittedToTacticAction = false;
            TacticModeSystem::Instance().ReportTacticActionFinished();
        }
    }
}

void Unit::AttackMoveUpdate()
{
    CheckCurrentAnimation(unitAnimations.m_walkAnimation);
}

void Unit::AttackUpdate()
{
    /// AttackState에서는 공격 후 다음 공격까지 남은 시간 동안 idleAnimation을 적용하므로 다른 방식으로 체크하는게 좋겠다.

    //RotateUnit(m_currentTargetUnit->GetTransform()->GetWorldPosition());

    //LookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition());
}

void Unit::ChaseUpdate()
{
    CheckCurrentAnimation(unitAnimations.m_walkAnimation);
}

void Unit::DeathUpdate()
{
    deathFunctionElapsed += Time::GetDeltaTime() * m_localTimeScale;

    if (m_burnEffect->IsDone() && m_unitSide == UnitSide::Enemy)
    {
        for (auto each : OnDeath)
        {
            each();
        }

        if (returnToPoolFunction != nullptr)
        {
            returnToPoolFunction();
        }
        deathFunctionElapsed = 0.0f;
        GetGameObject()->SetSelfActive(false);
    }
    else if (m_unitSide == UnitSide::Player)
    {
        /// Player일 경우
        if (deathFunctionElapsed >= deathAnimationDelay)
        {
            ChangeAnimation(unitAnimations.m_deathAnimation);
            GetGameObject()->SetSelfActive(false);
        }
    }
}

void Unit::StopAllStateTimer()
{
    for (auto each : m_stateTimerVector)
    {
        if (each)
            each->StopTimer();
    }
}

void Unit::ChangeAnimation(yunuGI::IAnimation* p_anim)
{

    if (m_latestChangedAnimation == p_anim)
        return;

    if (p_anim)
        m_animatorComponent->ChangeAnimation(p_anim, animationLerpDuration, animationTransitionSpeed);

    m_latestChangedAnimation = p_anim;
}

#pragma endregion

void Unit::CheckCurrentAnimation(yunuGI::IAnimation* currentStateAnimation)
{
    if (m_animatorComponent && m_animatorComponent->GetGI().GetCurrentAnimation() != currentStateAnimation)
    {
        ChangeAnimation(currentStateAnimation);
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

Unit::~Unit()
{
    if (!unitStatusUI.expired())
    {
        Scene::getCurrentScene()->DestroyGameObject(unitStatusUI.lock()->GetGameObject());
    }
}
int Unit::GetUnitDamage() const
{
    return m_autoAttackDamage;
}

void Unit::Damaged(Unit* opponentUnit, float opponentDmg)
{
    AddToOpponentObjectList(opponentUnit);
    DetermineHitDamage(opponentDmg);
    SetUnitCurrentHp(m_currentHealthPoint -= m_finalHitDamage);
    std::cout << this->GetUnitFbxName() << "Is Damaged By " << opponentUnit->GetUnitFbxName() << " Damage : " << m_finalHitDamage << std::endl;
    if (m_currentHealthPoint <= 0)
        m_currentResurrectingCount++;

    auto debuggingMesh = DebuggingMeshPool::Instance().Borrow();
    debuggingMesh->SetUnitObject(this);
    debuggingMesh->PopMeshUP(yunuGI::Color::green(), MaterialNum::Green);

    // ui로 표시되는, 혹은 최종 남은 체력은 반올림할 것인가 혹은 내림할 것인가는 아래에 구현.
}

void Unit::Damaged(float dmg)
{
    //DetermineHitDamage(dmg);
    //m_healthPoint -= m_finalHitDamage;
    SetUnitCurrentHp(m_currentHealthPoint -= dmg);
    if (m_currentHealthPoint <= 0)
        m_currentResurrectingCount++;

    auto debuggingMesh = DebuggingMeshPool::Instance().Borrow();
    debuggingMesh->SetUnitObject(this);
    debuggingMesh->PopMeshUP(yunuGI::Color::green(), MaterialNum::Green);
}

void Unit::Heal(float healingPoint)
{
    // 최대 체력이면 x
    SetUnitCurrentHp(m_currentHealthPoint += healingPoint);
    if (m_currentHealthPoint >= m_maxHealthPoint)
        SetUnitCurrentHp(m_maxHealthPoint);
}

void Unit::SetUnitCurrentHp(float p_newHp)
{
    m_currentHealthPoint = p_newHp;
    if (!unitStatusUI.expired())
    {
        unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_HP_Fill)->adjuster->SetTargetFloat(1 - m_currentHealthPoint / m_maxHealthPoint);
        unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_HP_Number_Current)->SetNumber(m_currentHealthPoint);
    }
    if (unitStatusPortraitUI)
    {
        unitStatusPortraitUI->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Fill)->adjuster->SetTargetFloat(m_currentHealthPoint / m_maxHealthPoint);
        unitStatusPortraitUI->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_PortraitBloodOverlay)->adjuster->SetTargetFloat(1 - m_currentHealthPoint / m_maxHealthPoint);
        unitStatusPortraitUI->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Number_Current)->SetNumber(m_currentHealthPoint);
        unitStatusPortraitUI->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Number_Max)->SetNumber(m_maxHealthPoint);
    }
}

float Unit::GetUnitCurrentHp() const
{
    return m_currentHealthPoint;
}

void Unit::IncreaseAttackPower(float p_attackPowerIncrease)
{
    /// 우선은 기본 공격력만 증가하는 로직입니다.
    if (m_autoAttackDamage + p_attackPowerIncrease <= m_maxAutoAttackDamage)
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
    SetUnitCurrentHp(m_maxHealthPoint);
    unitFSM.currentState = UnitState::Idle;
    m_currentTargetUnit = nullptr;
    m_opponentObjectSet.clear();
    m_recognizedThisSet.clear();
    m_attackingThisUnitSet.clear();
}

void Unit::UnitActionOnTacticModeEngaged()
{
    //unitAnimations.m_deathAnimation->Se
    //m_animatorComponent->GetGI().GetCurrentAnimation()->SetPlaySpeed(0.0f);
    StopAnimation();
    StopMove();
}

void Unit::UnitActionOnTacticModeEnded()
{
    ResumeAnimation();
}

void Unit::SetCurrentAnimationSpeed(yunuGI::IAnimation* p_anim, float p_speed)
{
    if (p_anim)
        p_anim->SetPlaySpeed(p_speed);
}

bool Unit::IsAllExtraPlayerUnitDead()
{
    bool temp = false;

    for (auto each : PlayerController::Instance().GetPlayerMap())
    {
        if (each.second == this)
            continue;

        if (each.second->GetCurrentUnitState() != UnitState::Death)
        {
            temp = false;
            break;
        }
        else
        {
            temp = true;
        }
    }

    return temp;
}

bool Unit::CheckEnemyStoppedByTacticMode() const
{
    return (!TacticModeSystem::Instance().IsUnitsPerformingCommand() || !TacticModeSystem::Instance().IsOrderingTimingNow());
}

void Unit::KnockBackUnit(Vector3d targetPosition, float knockBackDuration)
{
    knockBackStartPoint = GetGameObject()->GetTransform()->GetWorldPosition();
    m_navAgentComponent->Relocate(targetPosition);
    isFollowingNavAgent = false;
    MakeUnitPushedState(true);
    knockBackTimer->pushDuration = knockBackDuration;
    knockBackTimer->ActivateTimer();
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

void Unit::SetCurrentMovePosition(Vector3d p_pos)
{
    m_currentMovePosition = p_pos;
}

void Unit::SetWaveStartPosition(Vector3d p_pos)
{
    m_waveStartPosition = p_pos;
}

void Unit::PushMoveFunctionToTacticQueue(Vector3d p_pos)
{
    m_tacticModeQueue.push([=]()
        {
            OrderMove(p_pos);
        });
}

void Unit::PushAttackMoveFunctionToTacticQueue(Vector3d p_pos, Unit* p_selectedUnit)
{
    m_tacticModeQueue.push([=]()
        {
            OrderAttackMove(p_pos, p_selectedUnit);
            isTacticAttackMovePermitted = true;
        });
}

void Unit::PushAttackMoveFunctionToTacticQueue(Vector3d p_pos)
{
    m_tacticModeQueue.push([=]()
        {
            OrderAttackMove(p_pos);
        });
}

void Unit::PushSkillFunctionToTacticQueue(SkillEnum p_skillNum, Vector3d p_pos)
{
    m_tacticModeQueue.push([=]()
        {
            OrderSkill(p_skillNum, p_pos);
        });
}

void Unit::ReportTacticModeEngaged()
{
    /// 전술모드 진입 시 유닛의 행동 로직입니다.
    /// 이 때는 실제 Timescale을 멈춰도 되지만... 어차피 현재 선택된 플레이어는 시간이 흘러야 합니다...
    /// fsm transition 은 TacticModeSystem->IsOrderingTimingNow 로 막아주고,
    /// 여기선 현재 애니메이션의 Stop, unit이 갖고 있는 다른 컴포넌트에 대한 처리를 해줍니다.



}

bool Unit::IsTacticModeQueueEmpty() const
{
    return m_tacticModeQueue.empty();
}

void Unit::ChangeUnitStatRandomly()
{

}

void Unit::SetRessurectMaxCount(int p_cnt)
{
    m_resurrectingMaxCount = p_cnt;
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

    Vector3d movedPositionPerFrame = GetGameObject()->GetTransform()->GetWorldPosition() + (directionVector * m_speed * Time::GetDeltaTime() * m_localTimeScale);

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

void Unit::ResumeAnimation()
{
    SetCurrentAnimationSpeed(unitAnimations.m_idleAnimation, 1.0f);
    SetCurrentAnimationSpeed(unitAnimations.m_walkAnimation, 1.0f);
    SetCurrentAnimationSpeed(unitAnimations.m_attackAnimation, 1.0f);
    SetCurrentAnimationSpeed(unitAnimations.m_paralysisAnimation, 1.0f);
    SetCurrentAnimationSpeed(unitAnimations.m_deathAnimation, 1.0f);
}

void Unit::StopAnimation()
{
    SetCurrentAnimationSpeed(unitAnimations.m_idleAnimation, 0.0f);
    SetCurrentAnimationSpeed(unitAnimations.m_walkAnimation, 0.0f);
    SetCurrentAnimationSpeed(unitAnimations.m_attackAnimation, 0.0f);
    SetCurrentAnimationSpeed(unitAnimations.m_paralysisAnimation, 0.0f);
    SetCurrentAnimationSpeed(unitAnimations.m_deathAnimation, 0.0f);
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
    if (tauntingThisUnit = p_unit)
    {
        tauntingThisUnit = nullptr;
    }

    m_attackingThisUnitSet.erase(p_unit);

    /// 적군을 담고 있는 list에서 죽은 오브젝트 유닛을 빼준다.
    m_opponentObjectSet.erase(p_unit);
    DetermineCurrentTargetObject();
}

void Unit::SetPlayerSerialNumber(UnitType serialNum)
{
    playerSerialNumber = serialNum;
}

coroutine::Coroutine ShowPath(const std::vector<Vector3d> paths)
{
    for (int i = 0; i < paths.size(); i++)
    {
        DebugBeacon::PlaceBeacon(paths[i], yunuGI::Color::green(), Vector3d::one * 0.5, 0.5);
    }
    co_return;
}
void Unit::OrderMove(Vector3d position)
{
    StartCoroutine(ShowPath(m_unitNavField->GetSmoothPath(GetTransform()->GetWorldPosition(), position)));
    m_previousMovePosition = m_currentMovePosition;
    m_currentMovePosition = position;
    tauntingThisUnit = nullptr;
    //m_currentTargetUnit = nullptr;
    isAttackMoving = false;

    if (!(currentOrder == UnitState::WaveStart || currentOrder == UnitState::WaveMotion))
    {
        if (currentOrder != UnitState::Skill)
        {
            currentOrder = UnitState::Move;
        }
    }

    if (unitFSM.currentState == UnitState::OffsetMove)
    {
        SetUnitStateDirectly(UnitState::Move);
    }
}

// 유닛을 직접 마우스 우클릭했을 경우 
void Unit::OrderAttackMove(Vector3d position, Unit* p_selectedUnit)
{
    OrderAttackMove(position);
    isAttackMoving = false;
    tauntingThisUnit = p_selectedUnit;
    DetermineCurrentTargetObject();
}

void Unit::OrderAttackMove(Vector3d position)
{
    m_currentMovePosition = position;

    currentOrder = UnitState::AttackMove;
    dotween->DOLookAt(position, rotateTime, false);

    //PlayerController::Instance().SetRightClickFunction();
    // 다음 클릭은 Move로 바꿀 수 있도록 function 재정의.

    isAttackMoving = true;
}

void Unit::OrderSkill(SkillEnum p_skillNum, Vector3d position)
{
    if (m_unitSide == UnitSide::Player && m_playerSkillSystem && m_playerSkillSystem->IsSkillCoolingDown(p_skillNum))
    {
        // Player unit with valid skill system and skill not cooling down
        ExecuteSkillAction(position, p_skillNum);
    }
    else if (m_unitSide == UnitSide::Enemy)
    {
        // Non-player unit or player unit with skill cooling down or invalid skill system
        ExecuteSkillAction(position, p_skillNum);
    }
    PlayerController::Instance().SetCurrentPlayerSerialNumber(m_unitType);
    PlayerController::Instance().SetRightClickFunction();
}

void Unit::OrderSkill(SkillEnum p_skillNum)
{
    /// warrior 2nd active skill 처럼 캐릭터의 회전이 필요 없는 스킬
    currentOrder = UnitState::Skill;
    m_currentSelectedSkill = p_skillNum;

    PlayerController::Instance().SetLeftClickEmpty();
}

void Unit::ExecuteSkillAction(Vector3d p_pos, SkillEnum p_skillNum)
{
    currentOrder = UnitState::Skill;
    m_skillPosition = p_pos;
    m_currentSelectedSkill = p_skillNum;
    dotween->DOLookAt(p_pos, rotateTime, false);
    PlayerController::Instance().SetLeftClickEmpty();
    m_currentSkillPosition = p_pos;
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

NavigationField* Unit::GetNavField() const
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
        PlayerController::Instance().SetRightClickFunction();
    }
}
UIElement* Unit::GetBarBuffIcon(StatusEffect::StatusEffectEnum uiEnumID)
{
    if (unitStatusUI.expired())
    {
        return nullptr;
    }
    else
    {
        switch (uiEnumID)
        {
        case StatusEffect::StatusEffectEnum::Bleeding: return unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Buff_Bleeding);
        case StatusEffect::StatusEffectEnum::Blinding: return unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Buff_Blinding);
        case StatusEffect::StatusEffectEnum::Paralysis: return unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Buff_Paralysis);
        case StatusEffect::StatusEffectEnum::KnockBack: return unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Buff_KnockBack);
        case StatusEffect::StatusEffectEnum::Taunted: return unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Buff_Taunted);
        default: return nullptr;
        }
    }
}
UIElement* Unit::GetPortraitBuffIcon(StatusEffect::StatusEffectEnum uiEnumID)
{
    if (!unitStatusPortraitUI)
    {
        return nullptr;
    }
    else
    {
        switch (uiEnumID)
        {
        case StatusEffect::StatusEffectEnum::Bleeding: return unitStatusPortraitUI->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Buff_Bleeding);
        case StatusEffect::StatusEffectEnum::Blinding: return unitStatusPortraitUI->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Buff_Blinding);
        case StatusEffect::StatusEffectEnum::Paralysis: return unitStatusPortraitUI->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Buff_Paralysis);
        case StatusEffect::StatusEffectEnum::KnockBack: return unitStatusPortraitUI->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Buff_KnockBack);
        case StatusEffect::StatusEffectEnum::Taunted: return unitStatusPortraitUI->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Buff_Taunted);
        default: return nullptr;
        }
    }
}

void Unit::ReportStatusEffectApplied(StatusEffect::StatusEffectEnum p_effectType)
{
    if (auto ui = GetBarBuffIcon(p_effectType))
    {
        ui->EnableElement();
    }
    if (auto ui = GetPortraitBuffIcon(p_effectType))
    {
        ui->EnableElement();
    }
}

void Unit::ReportStatusEffectEnded(StatusEffect::StatusEffectEnum p_effectType)
{
    if (auto ui = GetBarBuffIcon(p_effectType))
    {
        ui->DisableElement();
    }
    if (auto ui = GetPortraitBuffIcon(p_effectType))
    {
        ui->DisableElement();
    }

}

void Unit::SetUnitStateDirectly(Unit::UnitState p_unitState)
{
    switch (p_unitState)
    {
    case Unit::UnitState::Idle:
    {
        unitFSM.SetUnitStateDirectly(p_unitState);
        break;
    }
    case Unit::UnitState::Move:
    {
        unitFSM.SetUnitStateDirectly(p_unitState);
        break;
    }
    case Unit::UnitState::Chase:
        break;
    case Unit::UnitState::Attack:
        break;
    case Unit::UnitState::AttackMove:
        break;
    case Unit::UnitState::Skill:
        break;
    case Unit::UnitState::Paralysis:
        break;
    case Unit::UnitState::Death:
        break;
    case Unit::UnitState::Resurrect:
        break;
    case Unit::UnitState::OffsetMove:
    {
        unitFSM.SetUnitStateDirectly(p_unitState);
        break;
    }
    case Unit::UnitState::WaveStart:
        break;
    case Unit::UnitState::WaveMotion:
        break;
    case Unit::UnitState::StateEnd:
        break;
    default:
        break;
    }
}

void Unit::PermitTacticAction()
{
    isPermittedToTacticAction = true;
}

bool Unit::GetJustCrushedState() const
{
    return isJustHitByQSkill;
}

bool Unit::IsUnitDead() const
{
    return (m_currentHealthPoint <= 0);
}

