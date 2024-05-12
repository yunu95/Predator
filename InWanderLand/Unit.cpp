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
#include "BurnEffect.h"
#include "CursorDetector.h"
#include "DebuggingMeshPool.h"
#include "StatusEffect.h"

void Unit::OnEnable()
{
    for (auto each : OnCreated)
    {
        each();
    }
}

void Unit::Start()
{
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

    std::function<bool()> trapClassifingFunction = [=]()
        {
            return (m_unitType == UnitType::SpikeTrap);
        };

    //m_currentSkillAnimation->ClearEvent();
    //unitAnimations.m_attackAnimation->ClearEvent();
    //GetTransform()->SetWorldPosition(startPosition);

    dotween = GetGameObject()->GetComponent<Dotween>();
    m_skillSystemComponent = GetGameObject()->GetComponent<SkillSystem>();
    m_burnEffect = GetGameObject()->GetComponent<BurnEffect>();
    m_animatorComponent = GetGameObject()->GetComponent<yunutyEngine::graphics::Animator>();

    //m_navAgentComponent = GetGameObject()->GetComponent<NavigationAgent>();

    //returnToPoolFunction = []() {};
    unitFSM.transitions[UnitState::Idle].push_back({ UnitState::Move,
        [this]() { return (currentOrder == UnitState::Move && !TacticModeSystem::Instance().IsUnitsPerformingCommand()) ||
        (currentOrder == UnitState::Move && TacticModeSystem::Instance().IsUnitsPerformingCommand()); } });

    unitFSM.transitions[UnitState::Idle].push_back({ UnitState::AttackMove,
        [this]() { return currentOrder == UnitState::AttackMove || (unitFSM.previousState == UnitState::Attack && isAttackMoving); } });

    unitFSM.transitions[UnitState::Idle].push_back({ UnitState::Chase,
        [this]() { return  (GameManager::Instance().IsWaveEngageMotionEnd() && (m_currentTargetUnit != nullptr &&
                            idleElapsed >= idleToChaseDelay) && m_currentTargetUnit->currentOrder != UnitState::Death &&
                            m_idDistance > 0.1f && m_atkDistance > 0.1f) && !TacticModeSystem::Instance().IsUnitsPerformingCommand(); } });

    unitFSM.transitions[UnitState::Move].push_back({ UnitState::Idle,
        [this]() { return currentOrder == UnitState::Idle; } });

    unitFSM.transitions[UnitState::Move].push_back({ UnitState::AttackMove,
        [this]() { return currentOrder == UnitState::AttackMove; } });

    unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Idle,
        [this]() { return abs(GetGameObject()->GetTransform()->GetWorldPosition().x - m_currentMovePosition.x) < 0.2f && abs(GetGameObject()->GetTransform()->GetWorldPosition().z - m_currentMovePosition.z) < 0.2f; } });

    unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Move,
        [this]() { return currentOrder == UnitState::Move; } });

    unitFSM.transitions[UnitState::AttackMove].push_back({ UnitState::Chase,
        [this]() { return m_currentTargetUnit != nullptr && isTacticAttackMovePermitted; } });

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
            return (m_currentTargetUnit == nullptr/*m_currentTargetUnit->GetCurrentUnitState() == UnitState::Death*/
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
        [=]() { return currentOrder == UnitState::Idle; } });

    for (int i = static_cast<int>(UnitState::Idle); i < static_cast<int>(UnitState::Skill); i++)
    {
        unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Skill,
        [=]() { return currentOrder == UnitState::Skill || trapClassifingFunction()
            && TacticModeSystem::Instance().IsOrderingTimingNow(); } });
    }

    for (int i = static_cast<int>(UnitState::Idle); i < static_cast<int>(UnitState::Paralysis); i++)
    {
        unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Paralysis,
        [this]() { return currentOrder == UnitState::Paralysis; } });
    }

    unitFSM.transitions[static_cast<UnitState>(UnitState::Paralysis)].push_back({ UnitState::Idle,
        [this]() { return currentOrder == UnitState::Idle; } });

    for (int i = static_cast<int>(UnitState::Idle); i < static_cast<int>(UnitState::Resurrect); i++)
    {
        unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Resurrect,
        [this]() { return m_currentHealthPoint <= 0 && m_resurrectingMaxCount >= m_currentResurrectingCount && !IsAllExtraPlayerUnitDead() && m_unitSide == UnitSide::Player; } });
    }

    unitFSM.transitions[static_cast<UnitState>(UnitState::Resurrect)].push_back({ UnitState::Idle,
    [this]() { return currentOrder == UnitState::Idle; } });

    for (int i = static_cast<int>(UnitState::Idle); i < static_cast<int>(UnitState::Death); i++)
    {
        unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Death,
        [this]() { return m_currentHealthPoint <= 0 && m_resurrectingMaxCount < m_currentResurrectingCount; } });
    }

    unitFSM.transitions[static_cast<UnitState>(UnitState::Idle)].push_back({ UnitState::OffsetMove,
    [this]() { return !GameManager::Instance().IsBattleSystemOperating() && m_unitSide == UnitSide::Player && m_unitType != UnitType::Warrior; } });

    unitFSM.transitions[static_cast<UnitState>(UnitState::OffsetMove)].push_back({ UnitState::WaveStart,
    [this]() { return GameManager::Instance().IsPlayerJustEnteredWaveRegion(); } });

    unitFSM.transitions[UnitState::Move].push_back({ UnitState::WaveStart,
    [this]() { return GameManager::Instance().IsPlayerJustEnteredWaveRegion(); } });

    unitFSM.transitions[static_cast<UnitState>(UnitState::WaveStart)].push_back({ UnitState::WaveMotion,
    [this]() { return GameManager::Instance().IsReadyToWaveEngageMotion(); } });

    unitFSM.transitions[static_cast<UnitState>(UnitState::WaveMotion)].push_back({ UnitState::Idle,
    [this]() { return currentOrder == UnitState::Idle; } });

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
    unitFSM.engageAction[UnitState::WaveStart] = [this]() { WaveStartEngage(); };
    unitFSM.engageAction[UnitState::WaveMotion] = [this]() { WaveMotionEngage(); };
    unitFSM.engageAction[UnitState::Resurrect] = [this]() { ResurrectEngage(); };

    unitFSM.updateAction[UnitState::Idle] = [this]() { IdleUpdate(); };
    unitFSM.updateAction[UnitState::Move] = [this]() { MoveUpdate(); };
    unitFSM.updateAction[UnitState::OffsetMove] = [this]() { OffsetMoveUpdate(); };
    unitFSM.updateAction[UnitState::AttackMove] = [this]() { AttackMoveUpdate(); };
    unitFSM.updateAction[UnitState::Chase] = [this]() { ChaseUpdate(); };
    unitFSM.updateAction[UnitState::Attack] = [this]() { AttackUpdate(); };
    unitFSM.updateAction[UnitState::Skill] = [this]() { SkillUpdate(); };
    unitFSM.updateAction[UnitState::Death] = [this]() { DeathUpdate(); };
    unitFSM.updateAction[UnitState::WaveStart] = [this]() { WaveStartUpdate(); };
    unitFSM.updateAction[UnitState::WaveMotion] = [this]() { WaveMotionUpdate(); };
    unitFSM.updateAction[UnitState::Resurrect] = [this]() { ResurrectUpdate(); };

    if (unitAnimations.m_attackAnimation != nullptr)
    {
        AttackSystem* atkSys = GetGameObject()->GetComponent<AttackSystem>();

        m_animatorComponent->PushAnimationWithFunc(unitAnimations.m_attackAnimation, attackTimingFrame, [=]()
            {
                if (m_currentTargetUnit != nullptr && currentOrder == UnitState::Attack)
                {
                    atkSys->Attack(m_currentTargetUnit, m_attackOffset);

                    if (isPermittedToTacticAction)
                    {
						isPermittedToTacticAction = false;
                        isTacticAttackMovePermitted = false;
						EnemyActionOnTacticModeEngaged();
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

    m_animatorComponent->Play(unitAnimations.m_idleAnimation);
}

void Unit::Update()
{
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
    if (m_unitSide == UnitSide::Player)
        PlayerController::Instance().ErasePlayerUnit(this);
}

void Unit::PlayFunction()
{

}

void Unit::StopFunction()
{
    if (!GetGameObject()->GetComponentWeakPtr<Unit>().expired())
    {
        yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
    }
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
    currentOrder = UnitState::Move;

    m_currentTargetUnit = nullptr;
    moveFunctionElapsed = 0.0f;

    m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::blue());

    m_navAgentComponent->SetSpeed(m_speed);
    m_navAgentComponent->MoveTo(m_currentMovePosition);

    ChangeAnimation(unitAnimations.m_walkAnimation);
}

void Unit::OffsetMoveEngage()
{
    StopMove();
    currentOrder = UnitState::OffsetMove;
    m_followingTargetUnit = PlayerController::Instance().GetPlayerMap().find(UnitType::Warrior)->second;
    isFollowing = false;
    currentOrder = UnitState::OffsetMove;
    moveFunctionElapsed = 0.0f;
    m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::green());
    m_navAgentComponent->SetSpeed(m_speed);
    ChangeAnimation(unitAnimations.m_idleAnimation);
}

void Unit::AttackMoveEngage()
{
    currentOrder = UnitState::AttackMove;

    moveFunctionElapsed = 0.0f;

    m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::gray());

    m_navAgentComponent->SetSpeed(m_speed);

    ChangeAnimation(unitAnimations.m_walkAnimation);
}

void Unit::AttackEngage()
{
    currentOrder = UnitState::Attack;

    m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::red());

    ChangeAnimation(unitAnimations.m_idleAnimation);

    AttackSystem* atkSys = GetGameObject()->GetComponent<AttackSystem>();
    isAttackAnimationOperating = false;
    isAnimationChangedAttackToIdle = false;

    attackFunctionElapsed = 0.0f;
    dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
    CheckCurrentAnimation(unitAnimations.m_idleAnimation);

    StopMove();
}

void Unit::ChaseEngage()
{
    currentOrder = UnitState::Chase;

    m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::green());

    dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);

    ChangeAnimation(unitAnimations.m_walkAnimation);
}

void Unit::SkillEngage()
{
    currentOrder = UnitState::Skill;
    ChangeAnimation(unitAnimations.m_idleAnimation);
    skillFunctionStartElapsed = 0.0f;
    int tempRand = rand() % 3 + 1;

    if (m_unitType == UnitType::Boss)
    {
        auto temp = GetGameObject()->GetComponent<BossSkillSystem>();
        temp->SelectSkillRandomly();
        m_currentSelectedSkill = temp->GetCurrentSelectedSkillNumber();
        if (m_currentTargetUnit != nullptr)
            m_currentSkillPosition = m_currentTargetUnit->GetTransform()->GetWorldPosition();
        else
            m_currentSkillPosition = PlayerController::Instance().GetPlayerMap().find(static_cast<UnitType>(tempRand))->second->GetTransform()->GetWorldPosition();
    }
    else if (m_unitType == UnitType::SpikeTrap || m_unitType == UnitType::ChessTrap)
    {
        m_currentSelectedSkill = SkillEnum::BossSkillOne;
    }
    else if (m_unitType == UnitType::MeleeEnemy)
    {
        m_currentSelectedSkill = SkillEnum::BossSkillOne;
        if (m_currentTargetUnit != nullptr)
            m_currentSkillPosition = m_currentTargetUnit->GetTransform()->GetWorldPosition();
        else
            m_currentSkillPosition = PlayerController::Instance().GetPlayerMap().find(static_cast<UnitType>(tempRand))->second->GetTransform()->GetWorldPosition();
    }
    else if (m_unitType == UnitType::RangedEnemy)
    {
        m_currentSelectedSkill = SkillEnum::BossSkillTwo;
        if (m_currentTargetUnit != nullptr)
            m_currentSkillPosition = m_currentTargetUnit->GetTransform()->GetWorldPosition();
        else
            m_currentSkillPosition = PlayerController::Instance().GetPlayerMap().find(static_cast<UnitType>(tempRand))->second->GetTransform()->GetWorldPosition();
    }

    if (m_unitType != UnitType::SpikeTrap && m_unitType != UnitType::ChessTrap)
        dotween->DOLookAt(m_currentSkillPosition, rotateTime, false);

    m_currentSelectedSkillEngageDelay = m_skillDurationMap.find(m_currentSelectedSkill)->second;
    m_currentSkillAnimation = m_skillAnimationMap.find(m_currentSelectedSkill)->second;
    m_selectedSkillTimingFrame = m_skillTimingFrameMap.find(m_currentSelectedSkill)->second;

    //m_currentSkillAnimation->SetEventFunc(m_selectedSkillTimingFrame, [=]()
    //	{
    //		m_skillSystemComponent->ActivateSkill(m_currentSelectedSkill, m_currentSkillPosition);
    //	});


    StopMove();
}

void Unit::ParalysisEngage()
{
    ChangeAnimation(unitAnimations.m_paralysisAnimation);
}

void Unit::DeathEngage()
{
    if (!unitStatusUI.expired())
        unitStatusUI.lock()->DisableElement();
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

void Unit::WaveStartEngage()
{
    if (!unitStatusUI.expired())
        unitStatusUI.lock()->EnableElement();
    currentOrder = UnitState::WaveStart;
    moveFunctionElapsed = 0.0f;
    ChangeAnimation(unitAnimations.m_walkAnimation);

    m_navAgentComponent->MoveTo(m_waveStartPosition);

    //dotween->DOLookAt(m_waveStartPosition, rotateTime, false);

    //m_navAgentComponent->SetActive(false);
    //dotween->DOMove(m_currentMovePosition, 1.0f).OnComplete([this]()
    //	{
    //		m_navAgentComponent->SetActive(true);
    //		m_navAgentComponent->AssignToNavigationField(m_unitNavField);
    //		m_navAgentComponent->Relocate(m_currentMovePosition);
    //		GameManager::Instance().ReportWaveStartStateEnd(this);
    //	});
}

void Unit::WaveMotionEngage()
{
    currentOrder = UnitState::WaveMotion;

    ChangeAnimation(unitAnimations.m_battleEngageAnimation);

    /*	if (m_unitType != UnitType::Warrior)
            dotween->DOLookAt(m_currentBelongingWavePosition, rotateTime, false);*/
}

void Unit::ResurrectEngage()
{
    currentOrder = UnitState::Resurrect;
    deathFunctionElapsed = 0.0f;
    ChangeAnimation(unitAnimations.m_deathAnimation);
    m_opponentObjectSet.clear();
    ReportUnitDeath();

    GetGameObject()->GetComponent<physics::Collider>()->SetActive(false);

    StopMove();
}

#pragma endregion


#pragma region State Update()
void Unit::IdleUpdate()
{
    CheckCurrentAnimation(unitAnimations.m_idleAnimation);

    if (!IsTacticModeQueueEmpty() && !TacticModeSystem::Instance().IsOrderingTimingNow() && isPermittedToTacticAction)
    {
        m_tacticModeQueue.front()();
        m_tacticModeQueue.pop();
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

    moveFunctionElapsed += Time::GetDeltaTime() * m_localTimeScale;

    if (moveFunctionElapsed >= moveFunctionCallDelay)
    {
        moveFunctionElapsed = 0.0f;
        //RotateUnit(m_currentMovePosition);
        dotween->DOLookAt(m_currentMovePosition, rotateTime, false);
        m_navAgentComponent->MoveTo(m_currentMovePosition);
    }

    ///
    if (abs(GetGameObject()->GetTransform()->GetWorldPosition().x - m_currentMovePosition.x) < 0.2f &&
        abs(GetGameObject()->GetTransform()->GetWorldPosition().z - m_currentMovePosition.z) < 0.2f)
    {
        currentOrder = UnitState::Idle;
        if (isPermittedToTacticAction)
        {
			isPermittedToTacticAction = false;
            TacticModeSystem::Instance().ReportTacticActionFinished();
        }
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
        ChangeAnimation(unitAnimations.m_walkAnimation);

        dotween->DOLookAt(m_followingTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
        m_navAgentComponent->MoveTo(m_followingTargetUnit->GetTransform()->GetWorldPosition());
        isFollowing = true;
    }
    else if (betweenUnitDistance <= m_stopFollowDinstance)
    {
        isFollowing = false;
        StopMove();
        ChangeAnimation(unitAnimations.m_idleAnimation);
    }
    else if (betweenUnitDistance <= m_followEngageDinstance && betweenUnitDistance >= m_stopFollowDinstance && isFollowing)
    {
        ChangeAnimation(unitAnimations.m_walkAnimation);

        dotween->DOLookAt(m_followingTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
        m_navAgentComponent->MoveTo(m_followingTargetUnit->GetTransform()->GetWorldPosition());
        isFollowing = true;
    }
}

void Unit::AttackMoveUpdate()
{
    CheckCurrentAnimation(unitAnimations.m_walkAnimation);

    moveFunctionElapsed += Time::GetDeltaTime() * m_localTimeScale;

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

    //RotateUnit(m_currentTargetUnit->GetTransform()->GetWorldPosition());

    //LookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition());

    if (!isAttackAnimationOperating)
    {
        if (!isAnimationChangedAttackToIdle)
        {
			ChangeAnimation(unitAnimations.m_idleAnimation);
            isAnimationChangedAttackToIdle = true;
        }

        attackFunctionElapsed += Time::GetDeltaTime() * m_localTimeScale;

        if (attackFunctionElapsed >= attackFunctionCallDelay)
        {
            DetermineCurrentTargetObject();
            attackFunctionElapsed = 0.0f;
            isAttackAnimationOperating = true;
            isAnimationChangedAttackToIdle = false;
            ChangeAnimation(unitAnimations.m_attackAnimation);
            CheckCurrentAnimation(unitAnimations.m_attackAnimation);
        }
        else if (attackFunctionElapsed < attackFunctionCallDelay)
        {
            dotween->DOLookAt(m_currentTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
            CheckCurrentAnimation(unitAnimations.m_idleAnimation);
        }
    }
}

void Unit::SkillUpdate()
{
    skillFunctionStartElapsed += Time::GetDeltaTime() * m_localTimeScale;

    if (skillFunctionStartElapsed >= m_currentSelectedSkillEngageDelay)
    {
        skillFunctionStartElapsed = 0.0f;
        ChangeAnimation(m_currentSkillAnimation);
    }
}

void Unit::ChaseUpdate()
{
    CheckCurrentAnimation(unitAnimations.m_walkAnimation);

    chaseFunctionElapsed += Time::GetDeltaTime() * m_localTimeScale;

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

void Unit::WaveStartUpdate()
{
    moveFunctionElapsed += Time::GetDeltaTime() * m_localTimeScale;
    float distance = (m_waveStartPosition - GetTransform()->GetWorldPosition()).Magnitude();

    if (abs(GetGameObject()->GetTransform()->GetWorldPosition().x - m_waveStartPosition.x) < 0.2f && abs(GetGameObject()->GetTransform()->GetWorldPosition().z - m_waveStartPosition.z) < 0.2f)
    {
        StopMove();
        GameManager::Instance().ReportWaveStartStateEnd(this);
    }
}

void Unit::WaveMotionUpdate()
{
    m_battleEngageMotionElapsed += Time::GetDeltaTime() * m_localTimeScale;

    float temp = unitAnimations.m_battleEngageAnimation->GetDuration();
    if (m_battleEngageMotionElapsed >= unitAnimations.m_battleEngageAnimation->GetDuration() + rotateTime)
    {
        m_battleEngageMotionElapsed = 0.0f;
        GameManager::Instance().ReportWaveMotionEnd(this);
        currentOrder = UnitState::Idle;
    }
}

void Unit::ResurrectUpdate()
{
    deathFunctionElapsed += Time::GetDeltaTime() * m_localTimeScale;

    if (deathFunctionElapsed >= m_resurrectingDuration)
    {
        currentOrder = UnitState::Idle;
        GetGameObject()->GetComponent<physics::Collider>()->SetActive(true);
        SetUnitCurrentHp(m_maxHealthPoint);
        ChangeUnitStatRandomly();
    }
}

void Unit::ChangeAnimation(yunuGI::IAnimation* p_anim)
{
    m_animatorComponent->ChangeAnimation(p_anim, animationLerpDuration, animationTransitionSpeed);
}

#pragma endregion

void Unit::CheckCurrentAnimation(yunuGI::IAnimation* currentStateAnimation)
{
    if (m_animatorComponent->GetGI().GetCurrentAnimation() != currentStateAnimation)
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

void Unit::SetUnitLocalTimeScale(float p_scale)
{
    m_localTimeScale = p_scale;
}

void Unit::EnemyActionOnTacticModeEngaged()
{
    //unitAnimations.m_deathAnimation->Se
    //m_animatorComponent->GetGI().GetCurrentAnimation()->SetPlaySpeed(0.0f);
	StopAnimation();
    StopMove();
}

void Unit::EnemyActionOnTacticModeEnded()
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

void Unit::RegisterSkillWithAnimation(SkillEnum p_enum)
{
    yunuGI::IAnimation* temp{ nullptr };

    switch (p_enum)
    {
    case Unit::SkillEnum::Q:
        temp = unitAnimations.m_skillOneAnimation;
        break;
    case Unit::SkillEnum::W:
        temp = unitAnimations.m_skillTwoAnimation;
        break;
    case Unit::SkillEnum::BossSkillOne:
        temp = unitAnimations.m_skillOneAnimation;
        break;
    case Unit::SkillEnum::BossSkillTwo:
        temp = unitAnimations.m_skillTwoAnimation;
        break;
    case Unit::SkillEnum::BossSkillThree:
        temp = unitAnimations.m_skillThreeAnimation;
        break;
    case Unit::SkillEnum::BossSkillFour:
        temp = unitAnimations.m_skillFourAnimation;
        break;
    default:
        break;
    }

    if (SkillSystem* skillsys = GetGameObject()->GetComponent<SkillSystem>();
        skillsys && temp)
    {
        m_animatorComponent = GetGameObject()->GetComponent<yunutyEngine::graphics::Animator>();

        m_animatorComponent->PushAnimationWithFunc(temp, m_skillTimingFrameMap.find(p_enum)->second, [=]()
            {
                skillsys->ActivateSkill(p_enum, m_currentSkillPosition);
            });
    }
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

void Unit::OrderMove(Vector3d position)
{
    m_previousMovePosition = m_currentMovePosition;
    m_currentMovePosition = position;
    tauntingThisUnit = nullptr;
    //m_currentTargetUnit = nullptr;
    isAttackMoving = false;

    if ((GameManager::Instance().IsBattleSystemOperating() || m_unitType == UnitType::Warrior) &&
        !(currentOrder == UnitState::WaveStart || currentOrder == UnitState::WaveMotion))
    {
        currentOrder = UnitState::Move;
        dotween->DOLookAt(position, rotateTime, false);
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

