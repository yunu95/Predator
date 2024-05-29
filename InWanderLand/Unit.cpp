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

void Unit::Update()
{
    if (stopFollowingNavAgentReference.expired())
        GetTransform()->SetWorldPosition(navAgentComponent.lock()->GetTransform()->GetWorldPosition());
    if (!unitStatusUI.expired())
        unitStatusUI.lock()->GetTransform()->SetWorldPosition(UIManager::Instance().GetUIPosFromWorld(GetTransform()->GetWorldPosition()));
    // 버프 적용
    for (auto& [buffID, buff] : buffs)
    {
        buff.get()->OnUpdate();
        buff.get()->durationLeft -= Time::GetDeltaTime();
    }
    std::erase_if(buffs, [](auto& buff) { return buff.durationLeft < 0; });
    // behaviour tree에 맞게 동작
    unitBehaviourTree.Update();
}

void Unit::OnDestroy()
{
    if (!unitStatusUI.expired())
        Scene::getCurrentScene()->DestroyGameObject(unitStatusUI.lock()->GetGameObject());
    if (!navAgentComponent.expired())
        Scene::getCurrentScene()->DestroyGameObject(navAgentComponent.lock()->GetGameObject());
}

void Unit::OnTransformUpdate()
{
}

coroutine::Coroutine Unit::Paralyze()
{
    return coroutine::Coroutine();
}

coroutine::Coroutine Unit::KnockBack()
{
    return coroutine::Coroutine();
}

coroutine::Coroutine Unit::KnockBack(bool yOffset)
{
    return coroutine::Coroutine();
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Death>()
{

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
void Unit::OnStateEngage<UnitBehaviourTree::Chasing>()
{
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Attack>()
{
    OnStateEngage<UnitBehaviourTree::Stop>();
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Move>()
{
    navAgentComponent.lock()->SetSpeed(unitTemplateData->pod.unitSpeed);
    navAgentComponent.lock()->MoveTo(currentMoveDestination);
    desiredRotation = GetTransform()->GetWorldPosition().GetAngleTo(currentMoveDestination);
    RotateUnit(currentMoveDestination);
    PlayAnimation(UnitAnimType::Move, true);
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::AttackMove>()
{
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
        unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_HP_Fill)->adjuster->SetTargetFloat(1 - currentHitPoint / maxHealthPoint);
        unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_HP_Number_Current)->SetNumber(currentHitPoint);
    }
    if (unitStatusPortraitUI)
    {
        unitStatusPortraitUI->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Fill)->adjuster->SetTargetFloat(currentHitPoint / maxHealthPoint);
        unitStatusPortraitUI->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_PortraitBloodOverlay)->adjuster->SetTargetFloat(1 - currentHitPoint / maxHealthPoint);
        unitStatusPortraitUI->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Number_Current)->SetNumber(currentHitPoint);
        unitStatusPortraitUI->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Number_Max)->SetNumber(maxHealthPoint);
    }
}

float Unit::GetUnitCurrentHp() const
{
    return currentHitPoint;
}

std::shared_ptr<Unit::Reference> Unit::AcquirePauseReference()
{
    return std::shared_ptr<Reference>();
}

std::shared_ptr<float> Unit::AcquireDamageFactor(float val)
{
    return std::shared_ptr<float>();
}

std::shared_ptr<float> Unit::AcquireDamageDelta(float val)
{
    return std::shared_ptr<float>();
}

std::shared_ptr<float> Unit::AcquireAttackSpeedFactor(float val)
{
    return std::shared_ptr<float>();
}

std::shared_ptr<float> Unit::AcquireCritFactor(float val)
{
    return std::shared_ptr<float>();
}
void Unit::Reset()
{
    SetUnitCurrentHp(maxHealthPoint);
    unitFSM.currentState = UnitState::Idle;
    currentTargetUnit = nullptr;
    opponentObjectSet.clear();
    recognizedThisSet.clear();
    attackingThisUnitSet.clear();
}
void Unit::KnockBackUnit(Vector3d targetPosition, float knockBackDuration)
{
    knockBackStartPoint = GetGameObject()->GetTransform()->GetWorldPosition();
    navAgentComponent.lock()->Relocate(targetPosition);
    isFollowingNavAgent = false;
    knockBackTimer->pushDuration = knockBackDuration;
    knockBackTimer->ActivateTimer();
}
void Unit::PlayAnimation(UnitAnimType animType, bool repeat)
{
    //if (latestChangedAnimation == p_anim)
        //return;

    //if (p_anim)
        //animatorComponent->ChangeAnimation(p_anim, animationLerpDuration, animationTransitionSpeed);

    //latestChangedAnimation = p_anim;
}

void Unit::RotateUnit(Vector3d endPosition)
{
    Vector3d startPosition = GetGameObject()->GetTransform()->GetWorldPosition();
    Vector3d objectFront = GetGameObject()->GetTransform()->GetWorldRotation().Forward();
    Vector3d distanceVec = endPosition - startPosition;

    float distance = distanceVec.Magnitude();

    Vector3d directionVector = distanceVec.Normalized();

    Vector3d movedPositionPerFrame = GetGameObject()->GetTransform()->GetWorldPosition() + (directionVector * speed * Time::GetDeltaTime() * localTimeScale);

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
void Unit::SetDesiredRotation(const Vector3d& facingDirection)
{
}
void Unit::UpdateRotation()
{
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
void Unit::ApplySkill(const Skill& skill)
{
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
void Unit::OrderSkill(const Skill& skill)
{
}
void Unit::ApplyBuff(const UnitBuff& skill)
{
}
void Unit::Init(const editor::Unit_TemplateData* templateData)
{
    switch (templateData->pod.playerUnitType)
    {
    case PlayerCharacterType::Robin:
        unitStatusPortraitUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin);
        break;
    case PlayerCharacterType::Ursula:
        unitStatusPortraitUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula);
        break;
    case PlayerCharacterType::Hansel:
        unitStatusPortraitUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel);
        break;
    default:
        break;
    }
    switch (templateData->pod.unitStatusBar)
    {
    case UnitStatusBarType::PLAYER:
        unitStatusUI = UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_Heroes));
        break;
    case UnitStatusBarType::ENEMY:
        unitStatusUI = UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_MeleeEnemy));
        break;
    }
    SetUnitCurrentHp(maxHealthPoint);
    if (!unitStatusUI.expired())
    {
        unitStatusUI.lock()->GetTransform()->SetWorldPosition(UIManager::Instance().GetUIPosFromWorld(GetTransform()->GetWorldPosition()));
        unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_HP_Cells)->adjuster->SetTargetFloat(maxHealthPoint);
        unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_HP_Number_Max)->SetNumber(maxHealthPoint);
    }
    burnEffect = GetGameObject()->GetComponent<BurnEffect>();
    animatorComponent = GetGameObject()->GetComponent<yunutyEngine::graphics::Animator>();
    if (navAgentComponent)
        navAgentComponent.lock()->SetSpeed(speed);
    if (animatorComponent)
        animatorComponent->Play(unitAnimations.idleAnimation);
}
void Unit::InitBehaviorTree()
{
}
void Unit::Reset()
{
}
void Unit::Birth()
{
}
const editor::Unit_TemplateData& Unit::GetUnitTemplateData()const
{
    return *unitTemplateData;
};
int Unit::GetTeamIndex() const
{
    return teamIndex;
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
bool Unit::IsUnitDead() const
{
    return (currentHitPoint <= 0);
}
