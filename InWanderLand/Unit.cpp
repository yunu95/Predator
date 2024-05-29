#include "InWanderLand.h"
#include "Unit.h"
#include "PlayerController.h"
#include "AttackSystem.h"
#include "RangedAttackSystem.h"
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
#include "UnitBuff.h"

template<std::weak_ptr<Reference> Unit::* referenceWeakptr>
std::shared_ptr<Reference> Unit::AcquireReference()
{
    if ((this->*referenceWeakptr).expired()) this->*referenceWeakptr = std::make_shared<Reference>();
    return this->*referenceWeakptr;
};
std::weak_ptr<Unit> Unit::GetClosestEnemy()
{
    auto minIt = std::min_element(attackRange.lock()->GetFoes().begin(), attackRange.lock()->GetFoes().end(), [this](const std::weak_ptr<Unit>& a, const std::weak_ptr<Unit>& b)
        {
            return (GetTransform()->GetWorldPosition() - a.lock()->GetTransform()->GetWorldPosition()).MagnitudeSqr() <
                (GetTransform()->GetWorldPosition() - b.lock()->GetTransform()->GetWorldPosition()).MagnitudeSqr();
        });
    if (minIt != attackRange.lock()->GetFoes().end())
        return *minIt;

    minIt = std::min_element(acquisitionRange.lock()->GetFoes().begin(), acquisitionRange.lock()->GetFoes().end(), [this](const std::weak_ptr<Unit>& a, const std::weak_ptr<Unit>& b)
        {
            return (GetTransform()->GetWorldPosition() - a.lock()->GetTransform()->GetWorldPosition()).MagnitudeSqr() <
                (GetTransform()->GetWorldPosition() - b.lock()->GetTransform()->GetWorldPosition()).MagnitudeSqr();
        });
    if (minIt != acquisitionRange.lock()->GetFoes().end())
        return *minIt;

    return std::weak_ptr<Unit>();
}
void Unit::Update()
{
    if (stopFollowingNavAgentReference.expired())
        GetTransform()->SetWorldPosition(navAgentComponent.lock()->GetTransform()->GetWorldPosition());
    if (blockRotationReference.expired())
        UpdateRotation();
    // UI가 유닛을 따라다니게 만듬
    auto& offset = unitTemplateData->pod.statusBarOffset;
    unitStatusUI.lock()->GetTransform()->SetWorldPosition(Vector3d{ offset.x,offset.y,0 }
    + UIManager::Instance().GetUIPosFromWorld(GetTransform()->GetWorldPosition()));
    // 버프 효과 적용
    for (auto& [buffID, buff] : buffs)
    {
        buff.get()->OnUpdate();
        buff.get()->durationLeft -= Time::GetDeltaTime();
    }
    std::erase_if(buffs, [](auto& buff) { return buff.durationLeft < 0; });
    // behaviour tree에 맞게 동작
    unitBehaviourTree.Update();
    lastPosition = GetTransform()->GetWorldPosition();
}

void Unit::OnDestroy()
{
    if (!unitStatusUI.expired())
        Scene::getCurrentScene()->DestroyGameObject(unitStatusUI.lock()->GetGameObject());
    if (!navAgentComponent.expired())
        Scene::getCurrentScene()->DestroyGameObject(navAgentComponent.lock()->GetGameObject());
}

template<>
void Unit::OnStateEngage<UnitBehaviourTree::Death>()
{
    onDeath();
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Paralysis>()
{
    PlayAnimation(UnitAnimType::Paralysis, true);
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Paralysis>()
{
    PlayAnimation(UnitAnimType::Paralysis, true);
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Pause>()
{
    PlayAnimation(UnitAnimType::Idle, true);
    OnStateEngage<UnitBehaviourTree::Stop>();
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Pause>()
{
    PlayAnimation(UnitAnimType::Idle, true);
    OnStateExit<UnitBehaviourTree::Stop>();
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Attack>()
{
    // Stop의 NavObstacle을 활성화하는 효과를 이용하여 공격을 할 때 장애물로 인식하게 만듭니다.
    OnStateEngage<UnitBehaviourTree::Stop>();
    Attack(GetClosestEnemy());
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Attack>()
{
    // Stop의 NavObstacle을 활성화하는 효과를 이용하여 공격을 할 때 장애물로 인식하게 만듭니다.
    OnStateExit<UnitBehaviourTree::Stop>();
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Move>()
{
    navAgentComponent.lock()->SetSpeed(unitTemplateData->pod.m_unitSpeed);
    PlayAnimation(UnitAnimType::Move, true);
}
template<>
void Unit::OnStateUpdate<UnitBehaviourTree::Move>()
{
    navAgentComponent.lock()->MoveTo(moveDestination);
    SetDesiredRotation(lastPosition - GetTransform()->GetWorldPosition());
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Stop>()
{
    navAgentComponent.lock()->SetSpeed(0.0f);
    navAgentComponent.lock()->MoveTo(GetTransform()->GetWorldPosition());
    navAgentComponent.lock()->SetActive(false);
    navObstacle.lock()->SetActive(true);
    PlayAnimation(UnitAnimType::Idle, true);
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Stop>()
{
    navAgentComponent.lock()->SetActive(true);
    navObstacle.lock()->SetActive(false);
}

Unit::~Unit()
{
    if (!unitStatusUI.expired())
    {
        Scene::getCurrentScene()->DestroyGameObject(unitStatusUI.lock()->GetGameObject());
    }
    if (!navAgentComponent.expired())
    {
        Scene::getCurrentScene()->DestroyGameObject(navAgentComponent.lock()->GetGameObject());
    }
}
bool Unit::IsInvincible() const
{
    return !invincibleReference.expired();
}
void Unit::Damaged(std::weak_ptr<Unit> opponentUnit, float opponentDmg)
{
    opponentUnit.lock()->onAttackHit();
    Damaged(opponentDmg);
}

void Unit::Damaged(float dmg)
{
    SetUnitCurrentHp(currentHitPoint -= dmg);
}

void Unit::Heal(float healingPoint)
{
    // 최대 체력이면 x
    SetUnitCurrentHp(currentHitPoint += healingPoint);
    if (currentHitPoint >= unitTemplateData->pod.max_Health)
        SetUnitCurrentHp(unitTemplateData->pod.max_Health);
}

void Unit::SetUnitCurrentHp(float p_newHp)
{
    currentHitPoint = p_newHp;
    if (!unitStatusUI.expired())
    {
        unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_HP_Fill)->adjuster->SetTargetFloat(1 - currentHitPoint / unitTemplateData->pod.max_Health);
        unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_HP_Number_Current)->SetNumber(currentHitPoint);
    }
    if (!unitStatusPortraitUI.expired())
    {
        unitStatusPortraitUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Fill)->adjuster->SetTargetFloat(currentHitPoint / unitTemplateData->pod.max_Health);
        unitStatusPortraitUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_PortraitBloodOverlay)->adjuster->SetTargetFloat(1 - currentHitPoint / unitTemplateData->pod.max_Health);
        unitStatusPortraitUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Number_Current)->SetNumber(currentHitPoint);
        unitStatusPortraitUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Number_Max)->SetNumber(unitTemplateData->pod.max_Health);
    }
}

float Unit::GetUnitCurrentHp() const
{
    return currentHitPoint;
}

std::shared_ptr<Reference> Unit::AcquirePauseReference()
{
    return AcquireReference<&Unit::pauseReference>();
}
void Unit::KnockBack(Vector3d targetPosition, float knockBackDuration)
{
    coroutineKnockBack = StartCoroutine(KnockBackCoroutine(targetPosition, knockBackDuration));
}

yunutyEngine::coroutine::Coroutine Unit::KnockBackCoroutine(Vector3d targetPosition, float knockBackDuration)
{
    co_return;
}

void Unit::Stun(float stunDuration)
{
    coroutineStun = StartCoroutine(StunCoroutine(stunDuration));
}
yunutyEngine::coroutine::Coroutine Unit::StunCoroutine(float stunDuration)
{
    co_return;
}
void Unit::PlayAnimation(UnitAnimType animType, bool repeat)
{
    //if (latestChangedAnimation == p_anim)
        //return;

    //if (p_anim)
        //animatorComponent->ChangeAnimation(p_anim, animationLerpDuration, animationTransitionSpeed);

    //latestChangedAnimation = p_anim;
}
float normalizeAngle(float angle) {
    while (angle < 0) angle += 360;
    while (angle >= 360) angle -= 360;
    return angle;
}
void Unit::SetDesiredRotation(const Vector3d& facingDirection)
{
    if (facingDirection.x == 0 && facingDirection.z == 0)
        return;
    desiredRotation = std::atan2(facingDirection.z, facingDirection.x) * math::Rad2Deg;
}
void Unit::UpdateRotation()
{
    currentRotation = normalizeAngle(currentRotation);
    desiredRotation = normalizeAngle(desiredRotation);
    float difference = desiredRotation - currentRotation;
    while (difference < -180) difference += 360;
    while (difference > 180) difference -= 360;


    if (difference > 0) {
        currentRotation = normalizeAngle(currentRotation + unitTemplateData->pod.rotationSpeed * Time::GetDeltaTime());
    }
    else {
        currentRotation = normalizeAngle(currentRotation - unitTemplateData->pod.rotationSpeed * Time::GetDeltaTime());
    }
    GetTransform()->SetWorldRotation(Vector3d(0, currentRotation, 0));
}

const UnitBehaviourTree& Unit::GetBehaviourTree() const
{
    // TODO: insert return statement here
}

void Unit::OnEnable()
{
    navAgentComponent.lock()->GetGameObject()->SetSelfActive(true);
}
void Unit::OnDisable()
{
    navAgentComponent.lock()->GetGameObject()->SetSelfActive(true);
}
void Unit::Start()
{
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
    StartCoroutine(ShowPath(SingleNavigationField::Instance().GetSmoothPath(GetTransform()->GetWorldPosition(), position)));
}
void Unit::OrderAttackMove(Vector3d position)
{
}
void Unit::OrderAttack(std::weak_ptr<Unit> opponent)
{
}
void Unit::OrderHold()
{
}
// 유닛과 관련된 객체들을 모조리 생성
void Unit::Init(const application::editor::Unit_TemplateData* unitTemplateData)
{
    this->unitTemplateData = unitTemplateData;
    switch (unitTemplateData->pod.playerUnitType)
    {
    case PlayerCharacterType::Robin:
        unitStatusPortraitUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)->GetWeakPtr<UIElement>();
        break;
    case PlayerCharacterType::Ursula:
        unitStatusPortraitUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)->GetWeakPtr<UIElement>();
        break;
    case PlayerCharacterType::Hansel:
        unitStatusPortraitUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel)->GetWeakPtr<UIElement>();
        break;
    default:
        break;
    }
    switch (unitTemplateData->pod.unitStatusBar)
    {
    case UnitStatusBarType::PLAYER:
        unitStatusUI = UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_Heroes));
        break;
    case UnitStatusBarType::ENEMY:
        unitStatusUI = UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_MeleeEnemy));
        break;
    }
    attackRange = GetGameObject()->AddGameObject()->AddComponentAsWeakPtr<UnitAcquisitionSphereCollider>();
    acquisitionRange = GetGameObject()->AddGameObject()->AddComponentAsWeakPtr<UnitAcquisitionSphereCollider>();
    animatorComponent = GetGameObject()->AddComponentAsWeakPtr<graphics::Animator>();
    navAgentComponent = Scene::getCurrentScene()->AddGameObject()->AddComponentAsWeakPtr<NavigationAgent>();
    navObstacle = navAgentComponent.lock()->GetGameObject()->AddComponentAsWeakPtr<NavigationObstacle>();
    burnEffect = GetGameObject()->AddComponentAsWeakPtr<BurnEffect>();
    teamIndex = unitTemplateData->pod.playerUnitType == PlayerCharacterType::None ? 2 : 1;
}
void Unit::Summon(const application::editor::UnitData* unitData)
{
    navAgentComponent.lock()->AssignToNavigationField(&SingleNavigationField::Instance());
    navAgentComponent.lock()->Relocate(Vector3d{ unitData->pod.position });
    Reset();
}
void Unit::Reset()
{
    SetUnitCurrentHp(unitTemplateData->pod.max_Health);
    DeleteCoroutine(coroutineKnockBack);
    DeleteCoroutine(coroutineStun);
    DeleteCoroutine(coroutineAttack);
    DeleteCoroutine(coroutineSkill);
    currentTargetUnit.reset();
    currentOrderType = UnitOrderType::Hold;
    pendingOrderType = UnitOrderType::Hold;
    teamIndex = 0;
    attackMoveDestination = moveDestination = GetGameObject()->GetTransform()->GetWorldPosition();
}
void Unit::InitBehaviorTree()
{
    // 이 행동 트리에 대한 설계문서는 Document/프로그래밍 폴더 내부의 파일 "InWanderLand Behaviour tree.drawio"입니다.
    unitBehaviourTree[UnitBehaviourTree::Death].enteringCondtion = [this]() { return !isAlive; };
    unitBehaviourTree[UnitBehaviourTree::Death].onEnter = [this]() { OnStateEngage<UnitBehaviourTree::Death>(); };
    unitBehaviourTree[UnitBehaviourTree::Paralysis].enteringCondtion = [this]() { return !paralysisReference.expired(); };
    unitBehaviourTree[UnitBehaviourTree::Paralysis].onEnter = [this]() { OnStateEngage<UnitBehaviourTree::Paralysis>(); };
    unitBehaviourTree[UnitBehaviourTree::Paralysis][UnitBehaviourTree::Knockback].enteringCondtion = [this]() { return !coroutineKnockBack.expired(); };
    unitBehaviourTree[UnitBehaviourTree::Paralysis][UnitBehaviourTree::Stun].enteringCondtion = [this]() { return !coroutineStun.expired(); };
    unitBehaviourTree[UnitBehaviourTree::Pause].enteringCondtion = [this]() { return !pauseReference.expired(); };
    unitBehaviourTree[UnitBehaviourTree::Chasing].enteringCondtion = [this]() {return CanProceedOrder<UnitOrderType::AttackUnit>(); };
    unitBehaviourTree[UnitBehaviourTree::Chasing].onEnter = [this]() {currentOrderType = pendingOrderType; };
    unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Attack].enteringCondtion = [this]() { return !attackRange.lock()->GetFoes().empty(); };
    unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Attack].onEnter = [this]() { OnStateEngage<UnitBehaviourTree::Attack>();  };
    unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Move].enteringCondtion = [this]() { return true; };
    unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Move].onEnter = [this]() {OnStateEngage<UnitBehaviourTree::Move>(); };
    unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Move].onUpdate = [this]()
        {
            moveDestination = GetAttackPosition(currentTargetUnit);
            OnStateEngage<UnitBehaviourTree::Move>();
        };
    unitBehaviourTree[UnitBehaviourTree::AttackMove].enteringCondtion = [this]() { return CanProceedOrder<UnitOrderType::AttackMove>(); };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Attack].enteringCondtion = [this]() { return !attackRange.lock()->GetFoes().empty(); };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Attack].onEnter = [this]() { OnStateEngage<UnitBehaviourTree::Attack>();  };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Move].enteringCondtion = [this]() { return !acquisitionRange.lock()->GetFoes().empty(); };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Move].onEnter = [this]() { OnStateEngage<UnitBehaviourTree::Move>(); };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Move].onUpdate = [this]() {
        currentTargetUnit = GetClosestEnemy();
        moveDestination = currentTargetUnit.expired() ? attackMoveDestination : GetAttackPosition(currentTargetUnit);
        };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Stop].enteringCondtion = [this]() { return true; };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Stop].onEnter = [this]() { OnStateEngage<UnitBehaviourTree::Stop>(); };
    unitBehaviourTree[UnitBehaviourTree::Move].enteringCondtion = [this]() {return CanProceedOrder<UnitOrderType::AttackMove>(); };
    unitBehaviourTree[UnitBehaviourTree::Move].onEnter = [this]() { OnStateEngage<UnitBehaviourTree::Move>(); };
    unitBehaviourTree[UnitBehaviourTree::Skill].enteringCondtion = [this]() {return !coroutineSkill.expired(); };
    unitBehaviourTree[UnitBehaviourTree::Hold].enteringCondtion = [this]() {return true; };
    unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Attack].enteringCondtion = [this]() {return !attackRange.lock()->GetFoes().empty(); };
    unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Attack].onEnter = [this]() {OnStateEngage<UnitBehaviourTree::Attack>(); };
    unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Stop].enteringCondtion = [this]() {return true; };
    unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Stop].onEnter = [this]() {OnStateEngage<UnitBehaviourTree::Stop>(); };
}
void Unit::Attack(std::weak_ptr<Unit> opponent)
{
}
Vector3d Unit::GetAttackPosition(std::weak_ptr<Unit> opponent)
{
    auto delta = opponent.lock()->GetTransform()->GetWorldPosition() - GetTransform()->GetWorldPosition();
    return opponent.lock()->GetTransform()->GetWorldPosition() - opponent.lock()->unitTemplateData->pod.collisionSize * delta.Normalized();
}
yunutyEngine::coroutine::Coroutine Unit::AttackCoroutine(std::weak_ptr<Unit> opponent)
{
    co_return;
}
// 유닛의 상태를 멀쩡한 상태로 원상복구
// 유닛의 동작에 필요한 인스턴스 중 dangling인 애들은 다시 생성 및 초기화,
void Unit::Reset()
{
    isAlive = true;
    SetUnitCurrentHp(unitTemplateData->pod.max_Health);
}
const editor::Unit_TemplateData& Unit::GetUnitTemplateData()const
{
    return *unitTemplateData;
};
int Unit::GetTeamIndex() const
{
    return teamIndex;
}
