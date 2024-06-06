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
#include "TacticModeSystem.h"
#include "IAnimation.h"
#include "SkillPreviewSystem.h"
#include "GameManager.h"
#include "BurnEffect.h"
#include "DebuggingMeshPool.h"
#include "StatusEffect.h"
#include "CinematicManager.h"
#include "UnitBuff.h"
#include "wanderResources.h"

coroutine::Coroutine ShowPath(const std::vector<Vector3d> paths)
{
    for (int i = 0; i < paths.size(); i++)
    {
        DebugBeacon::PlaceBeacon(paths[i], yunuGI::Color::green(), Vector3d::one * 0.5, 0.5);
    }
    co_return;
}
float getDeltaAngle(float difference) {
    while (difference < -180) difference += 360;
    while (difference > 180) difference -= 360;
    return difference;
}
std::weak_ptr<Unit> Unit::GetClosestEnemy()
{
    auto minIt = std::min_element(attackRange.lock()->GetEnemies().begin(), attackRange.lock()->GetEnemies().end(), [this](const Unit* const a, const Unit* const b)
        {
            return (GetTransform()->GetWorldPosition() - a->GetTransform()->GetWorldPosition()).MagnitudeSqr() <
                (GetTransform()->GetWorldPosition() - b->GetTransform()->GetWorldPosition()).MagnitudeSqr();
        });
    if (minIt != attackRange.lock()->GetEnemies().end())
        return (*minIt)->GetWeakPtr<Unit>();

    minIt = std::min_element(acquisitionRange.lock()->GetEnemies().begin(), acquisitionRange.lock()->GetEnemies().end(), [this](const Unit* const a, const Unit* const b)
        {
            return (GetTransform()->GetWorldPosition() - a->GetTransform()->GetWorldPosition()).MagnitudeSqr() <
                (GetTransform()->GetWorldPosition() - b->GetTransform()->GetWorldPosition()).MagnitudeSqr();
        });
    if (minIt != acquisitionRange.lock()->GetEnemies().end())
        return (*minIt)->GetWeakPtr<Unit>();

    return std::weak_ptr<Unit>();
}
void Unit::Update()
{
    if (referenceDisableNavAgent.BeingReferenced())
    {
        navAgentEnableFrameCount = 0;
        navAgentComponent.lock()->SetActive(false);
    }
    else
    {
        // navAgent 복구는 한 프레임 이후에 진행된다.
        if (navAgentEnableFrameCount >= 1)
            navAgentComponent.lock()->SetActive(!referenceDisableNavAgent.BeingReferenced());
        navAgentEnableFrameCount++;
    }
    navObstacle.lock()->SetActive(referenceEnableNavObstacle.BeingReferenced());
    //navObstacle.lock()->SetActive(false);

    if (!referenceBlockFollowingNavAgent.BeingReferenced())
        GetTransform()->SetWorldPosition(navAgentComponent.lock()->GetTransform()->GetWorldPosition());
    if (!referenceBlockRotation.BeingReferenced())
        UpdateRotation();
    // UI가 유닛을 따라다니게 만듬
    auto& offset = unitTemplateData->pod.statusBarOffset;
    if (!unitStatusUI.expired())
        unitStatusUI.lock()->GetTransform()->SetWorldPosition(Vector3d{ offset.x,offset.y,0 }
    + UIManager::Instance().GetUIPosFromWorld(GetTransform()->GetWorldPosition()));
    // 버프 효과 적용
    for (auto& [buffID, buff] : buffs)
    {
        buff.get()->OnUpdate();
        buff.get()->durationLeft -= Time::GetDeltaTime();
    }
    std::erase_if(buffs, [](std::pair<const UnitBuff::Type, std::shared_ptr<UnitBuff>>& pair)
        { return pair.second->durationLeft < 0; });
    // behaviour tree에 맞게 동작
    unitBehaviourTree.Update();
    auto navPos = navAgentComponent.lock()->GetTransform()->GetWorldPosition();
    auto navSpeed = navAgentComponent.lock()->GetSpeed();
    lastPosition = GetTransform()->GetWorldPosition();
    // 재생중인 애니메이션이 없다면 기본 애니메이션 출력
    // 어떤게 기본 애니메이션인지는 행동트리의 상태에 따라 바뀔 수 있다.
    if ((animatorComponent.lock()->IsDone() || blendWithDefaultAnimTrigger) && defaultAnimationType != UnitAnimType::None)
    {
        blendWithDefaultAnimTrigger = false;
        PlayAnimation(defaultAnimationType);
    }
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
    onStateEngage[UnitBehaviourTree::Death]();
    PlayAnimation(UnitAnimType::Death);
    defaultAnimationType = UnitAnimType::None;
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Paralysis>()
{
    onStateEngage[UnitBehaviourTree::Paralysis]();
    PlayAnimation(UnitAnimType::Paralysis, true);
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Paralysis>()
{
    onStateExit[UnitBehaviourTree::Paralysis]();
    PlayAnimation(UnitAnimType::Paralysis, true);
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Pause>()
{
    onStateEngage[UnitBehaviourTree::Pause]();
    PlayAnimation(UnitAnimType::Idle, true);
    SetNavObstacleActive(true);
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Pause>()
{
    onStateExit[UnitBehaviourTree::Pause]();
    PlayAnimation(UnitAnimType::Idle, true);
    SetNavObstacleActive(false);
}
template<>
void Unit::OnStateUpdate<UnitBehaviourTree::Chasing>()
{
    if (!referenceBlockPendingOrder.BeingReferenced() && !pendingTargetUnit.expired())
    {
        currentTargetUnit = pendingTargetUnit;
        pendingTargetUnit.reset();
    }
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Attack>()
{
    onStateEngage[UnitBehaviourTree::Attack]();
    // Stop의 NavObstacle을 활성화하는 효과를 이용하여 공격을 할 때 장애물로 인식하게 만듭니다.
    SetNavObstacleActive(true);
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Attack>()
{
    DeleteCoroutine(coroutineAttack);
    onStateExit[UnitBehaviourTree::Attack]();
    // Stop의 NavObstacle을 활성화하는 효과를 이용하여 공격을 할 때 장애물로 인식하게 만듭니다.
    SetNavObstacleActive(false);
}
template<>
void Unit::OnStateUpdate<UnitBehaviourTree::Attack>()
{
    if (currentTargetUnit.expired())
    {
        OrderHold();
        return;
    }
    SetDesiredRotation(currentTargetUnit.lock()->GetTransform()->GetWorldPosition() - GetTransform()->GetWorldPosition());
    if (!referenceBlockAttack.BeingReferenced())
        coroutineAttack = StartCoroutine(AttackCoroutine(currentTargetUnit));
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Move>()
{
    onStateEngage[UnitBehaviourTree::Move]();
    navAgentComponent.lock()->SetSpeed(unitTemplateData->pod.m_unitSpeed);
    StartCoroutine(ShowPath(SingleNavigationField::Instance().GetSmoothPath(GetTransform()->GetWorldPosition() + GetTransform()->GetWorldRotation().Forward() * unitTemplateData->pod.collisionSize, moveDestination)));
    PlayAnimation(UnitAnimType::Move, true);
}
template<>
void Unit::OnStateUpdate<UnitBehaviourTree::Move>()
{
    SetNavObstacleActive(false);
    static constexpr float epsilon = 0.1f;
    if ((moveDestination - GetTransform()->GetWorldPosition()).MagnitudeSqr() < epsilon)
    {
        OrderAttackMove(moveDestination);
    }
    navAgentComponent.lock()->MoveTo(moveDestination);
    SetDesiredRotation(GetTransform()->GetWorldPosition() - lastPosition);
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Skill>()
{
    onStateEngage[UnitBehaviourTree::Skill]();
    currentOrderType = UnitOrderType::Skill;
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::SkillOnGoing>()
{
    DeleteCoroutine(coroutineSkill);
    onGoingSkill.reset();
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::SkillCasting>()
{
    assert(pendingSkill.get() != nullptr);
    SetDesiredRotation(pendingSkill.get()->targetPos - GetTransform()->GetWorldPosition());
    onGoingSkill = std::move(pendingSkill);
    coroutineSkill = StartCoroutine(onGoingSkill.get()->operator()());
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Stop>()
{
    onStateEngage[UnitBehaviourTree::Stop]();
    PlayAnimation(UnitAnimType::Idle, true);
    SetNavObstacleActive(true);
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Stop>()
{
    onStateExit[UnitBehaviourTree::Stop]();
    SetNavObstacleActive(false);
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
bool Unit::IsPlayerUnit() const
{
    return unitTemplateData->pod.playerUnitType.enumValue != PlayerCharacterType::None;
}
bool Unit::IsInvulenerable() const
{
    return referenceInvulnerable.BeingReferenced();
}
bool Unit::IsAlive() const
{
    return isAlive;
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
    currentHitPoint = std::fmax(0, p_newHp);
    if (p_newHp <= 0)
    {
        isAlive = false;
    }
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

void Unit::KnockBack(Vector3d targetPosition, float knockBackDuration)
{
    DeleteCoroutine(coroutineKnockBack);
    coroutineKnockBack = StartCoroutine(KnockBackCoroutine(targetPosition, knockBackDuration));
}

void Unit::KnockBackRelativeVector(Vector3d relativeVector, float knockBackDuration)
{
    DeleteCoroutine(coroutineKnockBack);
    coroutineKnockBack = StartCoroutine(KnockBackCoroutine(relativeVector, knockBackDuration, true));
}

void Unit::Paralyze(float paralyzeDuration)
{
    StartCoroutine(referenceParalysis.AcquireForSecondsCoroutine(paralyzeDuration));
}

yunutyEngine::coroutine::Coroutine Unit::KnockBackCoroutine(Vector3d targetPosition, float knockBackDuration, bool relative)
{
    auto blockFollowingNavAgent = referenceBlockFollowingNavAgent.Acquire();
    auto paralyzed = referenceParalysis.Acquire();
    coroutine::ForSeconds forSeconds{ knockBackDuration };
    const auto& constant = GlobalConstant::GetSingletonInstance().pod;
    constant.gravitySpeed;
    // y = vy0 * t - 0.5 * g * t^2
    // vy0 = (0 + 0.5 * g * t^2) / t
    float vy0 = 0.5 * constant.gravitySpeed * knockBackDuration;
    float y;
    Vector3d startPos = GetTransform()->GetWorldPosition();

    if (relative)
    {
        targetPosition = startPos + targetPosition;
    }

    navAgentComponent.lock()->Relocate(targetPosition);
    navAgentComponent.lock()->MoveTo(targetPosition);

    while (forSeconds.Tick())
    {
        y = vy0 * forSeconds.Elapsed() - 0.5 * constant.gravitySpeed * forSeconds.Elapsed() * forSeconds.Elapsed();
        Vector3d pos = Vector3d::Lerp(startPos, navAgentComponent.lock()->GetTransform()->GetWorldPosition(), forSeconds.ElapsedNormalized());
        pos.y = y;
        GetTransform()->SetWorldPosition(pos);
        co_await std::suspend_always{};
    }
    co_return;
}
void Unit::PlayAnimation(UnitAnimType animType, bool repeat)
{
    auto anim = wanderResources::GetAnimation(unitTemplateData->pod.skinnedFBXName, animType);

    if (animatorComponent.lock()->GetGI().GetCurrentAnimation() == nullptr || animatorComponent.lock()->GetGI().GetCurrentAnimation() == anim)
    {
        animatorComponent.lock()->Play(anim);
    }
    else
    {
        animatorComponent.lock()->ChangeAnimation(anim, GlobalConstant::GetSingletonInstance().pod.defaultAnimBlendTime, 1);
    }
    if (repeat)
    {
        blendWithDefaultAnimTrigger = true;
        defaultAnimationType = animType;
        //defaultAnimation = anim;
    }
}
void Unit::BlendWithDefaultAnimation()
{
    blendWithDefaultAnimTrigger = true;
}
void Unit::SetDefaultAnimation(UnitAnimType animType)
{
    defaultAnimationType = animType;
}
float normalizeAngle(float angle) {
    while (angle < 0) angle += 360;
    while (angle >= 360) angle -= 360;
    return angle;
}
void Unit::SetDesiredRotation(const Vector3d& facingDirection)
{
    static constexpr float epsilon = 1.0f;
    if (facingDirection.MagnitudeSqr() < epsilon * Time::GetDeltaTime() * Time::GetDeltaTime())
        return;
    desiredRotation = std::atan2(facingDirection.z, facingDirection.x) * math::Rad2Deg;
}
std::weak_ptr<coroutine::Coroutine> Unit::SetRotation(const Vector3d& facingDirection, float rotatingTime)
{
    desiredRotation = std::atan2(facingDirection.z, facingDirection.x) * math::Rad2Deg;
    return SetRotation(desiredRotation, rotatingTime);
}
std::weak_ptr<coroutine::Coroutine> Unit::SetRotation(float facingAngle, float rotatingTime)
{
    return StartCoroutine(SettingRotation(facingAngle, rotatingTime));
}
coroutine::Coroutine Unit::SettingRotation(float facingAngle, float rotatingTime)
{
    auto pauseGuard = referencePause.Acquire();
    desiredRotation = facingAngle;
    if (rotatingTime != 0)
    {
        currentRotationSpeed = std::fabs(getDeltaAngle(desiredRotation - currentRotation)) / rotatingTime;
    }
    else
    {
        currentRotationSpeed = 1000000000;
    }
    co_yield coroutine::WaitForSeconds(rotatingTime);
    currentRotationSpeed = unitTemplateData->pod.rotationSpeed;
}
void Unit::SetNavObstacleActive(bool active)
{
    if (active)
    {
        enableNavObstacle = referenceEnableNavObstacle.Acquire();
        disableNavAgent = referenceDisableNavAgent.Acquire();
        //navAgentComponent.lock()->SetActive(false);
        //navObstacle.lock()->SetActive(true);
    }
    else
    {
        enableNavObstacle.reset();
        disableNavAgent.reset();
        //navObstacle.lock()->SetActive(false);
        //navAgentComponent.lock()->SetActive(true);
    }
}
void Unit::UpdateRotation()
{
    currentRotation = normalizeAngle(currentRotation);
    desiredRotation = normalizeAngle(desiredRotation);
    float difference = desiredRotation - currentRotation;
    difference = getDeltaAngle(difference);

    if (std::fabs(difference) < currentRotationSpeed * Time::GetDeltaTime()) {
        currentRotation = desiredRotation;
    }
    else if (difference > 0) {
        currentRotation = normalizeAngle(currentRotation + currentRotationSpeed * Time::GetDeltaTime());
    }
    else {
        currentRotation = normalizeAngle(currentRotation - currentRotationSpeed * Time::GetDeltaTime());
    }
    GetTransform()->SetWorldRotation(Vector3d(0, -currentRotation + 90, 0));
}
void Unit::OnContentsStop()
{
    UnitPool::SingleInstance().Return(GetWeakPtr<Unit>());
}
void Unit::OnEnable()
{
    navAgentComponent.lock()->GetGameObject()->SetSelfActive(true);
}
void Unit::OnDisable()
{
    navAgentComponent.lock()->GetGameObject()->SetSelfActive(false);
}
void Unit::Start()
{
}
void Unit::Relocate(const Vector3d& pos)
{
    navAgentComponent.lock()->Relocate(pos);
    //OrderHold();
}
void Unit::OrderMove(Vector3d position)
{
    pendingOrderType = UnitOrderType::Move;
    moveDestination = position;
}
void Unit::OrderAttackMove(Vector3d position)
{
    pendingOrderType = UnitOrderType::AttackMove;
    attackMoveDestination = position;
}
void Unit::OrderAttack(std::weak_ptr<Unit> opponent)
{
    pendingOrderType = UnitOrderType::AttackUnit;
    pendingTargetUnit = opponent;
}
void Unit::OrderHold()
{
    pendingOrderType = UnitOrderType::Hold;
}
// 유닛과 관련된 객체들을 모조리 생성
void Unit::Init(const application::editor::Unit_TemplateData* unitTemplateData)
{
    this->unitTemplateData = unitTemplateData;
    attackRange = GetGameObject()->AddGameObject()->AddComponentAsWeakPtr<UnitAcquisitionSphereCollider>();
    acquisitionRange = GetGameObject()->AddGameObject()->AddComponentAsWeakPtr<UnitAcquisitionSphereCollider>();
    attackRange.lock()->owner = GetWeakPtr<Unit>();
    acquisitionRange.lock()->owner = GetWeakPtr<Unit>();
    navAgentComponent = Scene::getCurrentScene()->AddGameObject()->AddComponentAsWeakPtr<NavigationAgent>();
    navObstacle = navAgentComponent.lock()->GetGameObject()->AddComponentAsWeakPtr<NavigationObstacle>();
    navObstacle.lock()->SetObstacleType(NavigationObstacle::ObstacleType::Cylinder);
    burnEffect = GetGameObject()->AddComponentAsWeakPtr<BurnEffect>();
    teamIndex = unitTemplateData->pod.playerUnitType.enumValue == PlayerCharacterType::None ? 2 : PlayerController::playerTeamIndex;
    skinnedMeshGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(unitTemplateData->pod.skinnedFBXName);
    animatorComponent = skinnedMeshGameObject->GetComponentWeakPtr<graphics::Animator>();
    skinnedMeshGameObject->SetParent(GetGameObject());
    skinnedMeshGameObject->GetTransform()->SetLocalPosition(Vector3d::zero);
    skinnedMeshGameObject->GetTransform()->SetLocalRotation(Quaternion{ {0,180,0} });
    skinnedMeshGameObject->GetTransform()->SetLocalScale(Vector3d::one);
    //wanderResources::PushAnimations(animatorComponent.lock().get(), unitTemplateData->pod.skinnedFBXName);
    unitCollider = GetGameObject()->AddComponentAsWeakPtr<physics::SphereCollider>();
    auto rigidBody = GetGameObject()->AddComponentAsWeakPtr<physics::RigidBody>();
    rigidBody.lock()->SetAsKinematic(true);
    InitBehaviorTree();
    // 애니메이션에 이벤트 삽입
    auto& ptm = particle::ParticleTool_Manager::GetSingletonInstance();
    const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    std::wstring fbxNameW = yutility::GetWString(unitTemplateData->pod.skinnedFBXName);
    //fbxNameW.assign(pod.templateData->pod.skinnedFBXName.begin(), pod.templateData->pod.skinnedFBXName.end());

    /// Particle Setting
    for (auto& eachPI : ptm.GetChildrenParticleInstanceList(unitTemplateData->pod.skinnedFBXName))
    {
        auto pObj = GetGameObject()->AddGameObject();
        auto sptr = eachPI.lock();
        pObj->GetTransform()->SetLocalPosition(sptr->offsetPos);
        pObj->GetTransform()->SetLocalRotation(sptr->rotation);
        pObj->GetTransform()->SetLocalScale(sptr->scale);
        pObj->setName(sptr->name);
        auto pr = pObj->AddComponent<graphics::ParticleRenderer>();
        pr->SetParticleShape((yunutyEngine::graphics::ParticleShape)sptr->particleData.shape);
        pr->SetParticleMode((yunutyEngine::graphics::ParticleMode)sptr->particleData.particleMode);
        pr->SetLoop(sptr->particleData.isLoop);
        pr->SetDuration(sptr->particleData.duration);
        pr->SetLifeTime(sptr->particleData.lifeTime);
        pr->SetSpeed(sptr->particleData.speed);
        pr->SetStartScale(sptr->particleData.startScale);
        pr->SetEndScale(sptr->particleData.endScale);
        pr->SetMaxParticle(sptr->particleData.maxParticle);
        pr->SetPlayAwake(sptr->particleData.playAwake);
        pr->SetRadius(sptr->particleData.radius);
        pr->SetAngle(sptr->particleData.angle);

        pr->SetRateOverTime(sptr->particleData.rateOverTime);

        pr->SetBurstsCount(sptr->particleData.burstsCount);
        pr->SetInterval(sptr->particleData.interval);

        std::wstring texturePath;
        texturePath.assign(sptr->particleData.texturePath.begin(), sptr->particleData.texturePath.end());
        auto texturePtr = resourceManager->GetTexture(texturePath);
        if (texturePtr)
        {
            pr->SetTexture(texturePtr);
        }

        pObj->SetSelfActive(false);
    }

    /// Animation Event Setting
    auto& list = resourceManager->GetFBXAnimationList(fbxNameW);
    for (auto& each : list)
    {
        std::string aniName;
        aniName.assign(each->GetName().begin(), each->GetName().end());

        for (auto& eventWeak : ptm.GetAnimationEventList(ptm.GetMatchingIAnimation(unitTemplateData->pod.skinnedFBXName, aniName)))
        {
            auto event = eventWeak.lock();
            auto type = event->GetType();
            switch (type)
            {
            case application::AnimationEventType::GameObject_ActivateEvent:
            {
                auto ptr = static_cast<GameObject_ActivateEvent*>(event.get());
                GameObject* particle = nullptr;
                for (auto& child : GetGameObject()->GetChildren())
                {
                    if (child->getName() == ptr->objName)
                    {
                        particle = child;
                        break;
                    }
                }

                animatorComponent.lock()->PushAnimationWithFunc(each, event->frame, [=]()
                    {
                        particle->SetSelfActive(true);
                        auto ptr = particle->GetComponent<graphics::ParticleRenderer>();
                        ptr->Play();
                    });

                break;
            }
            case application::AnimationEventType::GameObject_DisabledEvent:
            {
                auto ptr = static_cast<GameObject_DisabledEvent*>(event.get());
                GameObject* particle = nullptr;
                for (auto& child : GetGameObject()->GetChildren())
                {
                    if (child->getName() == ptr->objName)
                    {
                        particle = child;
                        break;
                    }
                }

                animatorComponent.lock()->PushAnimationWithFunc(each, event->frame, [=]()
                    {
                        particle->SetSelfActive(false);
                    });

                break;
            }
            case application::AnimationEventType::GameObject_TransformEditEvent:
            {
                auto ptr = static_cast<GameObject_TransformEditEvent*>(event.get());
                GameObject* particle = nullptr;
                for (auto& child : GetGameObject()->GetChildren())
                {
                    if (child->getName() == ptr->objName)
                    {
                        particle = child;
                        break;
                    }
                }

                for (int i = 0; i < each->GetTotalFrame(); i++)
                {
                    animatorComponent.lock()->PushAnimationWithFunc(each, i, [=]()
                        {
                            auto& aem = AnimationEventManager::GetSingletonInstance();
                            auto target = aem.GetLerpPoint(ptr->editData, i);
                            particle->GetTransform()->SetLocalPosition(target->GetLocalPosition());
                            particle->GetTransform()->SetLocalRotation(target->GetLocalRotation());
                            particle->GetTransform()->SetLocalScale(target->GetLocalScale());
                        });
                }

                break;
            }
            case application::AnimationEventType::Sound_PlayOnceEvent:
            {
                auto ptr = static_cast<Sound_PlayOnceEvent*>(event.get());
                animatorComponent.lock()->PushAnimationWithFunc(each, event->frame, [=]()
                    {
                        yunutyEngine::SoundSystem::PlaySoundfile3D(ptr->rscPath, animatorComponent.lock()->GetGameObject()->GetTransform()->GetWorldPosition());
                    });
                break;
            }
            case application::AnimationEventType::GameObject_AwakeEvent:
            {
                auto ptr = static_cast<GameObject_AwakeEvent*>(event.get());
                GameObject* particle = nullptr;
                for (auto& child : GetGameObject()->GetChildren())
                {
                    if (child->getName() == ptr->objName)
                    {
                        particle = child;
                        break;
                    }
                }

                animatorComponent.lock()->PushAnimationWithFunc(each, event->frame, [=]()
                    {
                        auto ptr = particle->GetComponent<graphics::ParticleRenderer>();
                        ptr->Reset();
                    });

                break;
            }
            default:
                break;
            }
        }
    }
}
void Unit::Summon(const application::editor::UnitData* unitData)
{
    switch (unitTemplateData->pod.playerUnitType.enumValue)
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
    switch (unitTemplateData->pod.unitStatusBar.enumValue)
    {
    case UnitStatusBarType::PLAYER:
        unitStatusUI = UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_Heroes));
        break;
    case UnitStatusBarType::ENEMY:
        unitStatusUI = UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_MeleeEnemy));
        break;
    }
    this->unitData = unitData;
    currentRotationSpeed = unitTemplateData->pod.rotationSpeed;
    onAttack = unitData->onAttack;
    // 내가 때린 공격이 적에게 맞았을 때, 근거리 공격인 경우라면 onAttack과 호출시점이 같겠으나 원거리 공격인 경우에는 시간차가 있을 수 있다. 
    onAttackHit = unitData->onAttackHit;
    // 내가 피해를 입었을 때
    onDamaged = unitData->onDamaged;
    // 유닛이 새로 생성될 때
    onCreated = unitData->onCreated;
    // 유닛이 회전을 끝냈을 때
    onRotationFinish = unitData->onRotationFinish;
    onStateEngage = unitData->onStateEngage;
    onStateExit = unitData->onStateExit;

    GetTransform()->SetWorldPosition(Vector3d{ unitData->pod.position });
    navAgentComponent.lock()->GetTransform()->SetWorldPosition(Vector3d{ unitData->pod.position });
    navAgentComponent.lock()->AssignToNavigationField(&SingleNavigationField::Instance());
    //navAgentComponent.lock()->Relocate(Vector3d{ unitData->pod.position })
    navObstacle.lock()->AssignToNavigationField(&SingleNavigationField::Instance());
    navObstacle.lock()->SetRadiusAndHeight(unitTemplateData->pod.collisionSize, 100);

    unitCollider.lock()->SetRadius(unitTemplateData->pod.collisionSize);
    attackRange.lock()->SetRadius(unitTemplateData->pod.m_atkRadius);
    attackRange.lock()->SetColor(yunuGI::Color::red());
    acquisitionRange.lock()->SetRadius(unitTemplateData->pod.m_idRadius);
    acquisitionRange.lock()->SetColor(yunuGI::Color::gray());
    // 공격 범위
    //AttachDebugMesh(attackRange.lock()->GetGameObject()->AddGameObject(), DebugMeshType::Sphere)->GetTransform()->SetLocalScale(Vector3d{ 1,0.25,1 } *2 * unitTemplateData->pod.m_atkRadius);
    // 인식 범위
    //AttachDebugMesh(acquisitionRange.lock()->GetGameObject()->AddGameObject(), DebugMeshType::Sphere)->GetTransform()->SetLocalScale(Vector3d{ 1,0.25,1 } *2 * unitTemplateData->pod.m_idRadius);
    // 혹여나 플레이어 캐릭터라면 플레이어로 등록, 플레이어가 아니면 그냥 무시된다.
    switch (unitData->pod.templateData->pod.unitControllerType.enumValue)
    {
    case UnitControllerType::PLAYER:
    {
        PlayerController::Instance().RegisterUnit(GetWeakPtr<Unit>());
        controllers.push_back(&PlayerController::Instance());
        break;
    }
    case UnitControllerType::ANGRY_MOB:
    {
        EnemyAggroController::Instance().RegisterUnit(GetWeakPtr<Unit>());
        controllers.push_back(&EnemyAggroController::Instance());
        break;
    }
    case UnitControllerType::RANGED_ELITE:
    {
        EnemyAggroController::Instance().RegisterUnit(GetWeakPtr<Unit>());
        controllers.push_back(&EnemyAggroController::Instance());
        break;
    }
    }

    Reset();
    onCreated();
}
void Unit::Reset()
{
    isAlive = true;
    SetUnitCurrentHp(unitTemplateData->pod.max_Health);
    DeleteCoroutine(coroutineKnockBack);
    DeleteCoroutine(coroutineAttack);
    DeleteCoroutine(coroutineSkill);
    currentTargetUnit.reset();
    currentOrderType = UnitOrderType::AttackMove;
    pendingOrderType = UnitOrderType::AttackMove;
    attackMoveDestination = moveDestination = GetGameObject()->GetTransform()->GetWorldPosition();
}
void Unit::InitBehaviorTree()
{
    // 이 행동 트리에 대한 설계문서는 Document/프로그래밍 폴더 내부의 파일 "InWanderLand Behaviour tree.drawio"입니다.
    unitBehaviourTree[UnitBehaviourTree::Death].enteringCondtion = [this]()
        {
            return !isAlive;
        };
    unitBehaviourTree[UnitBehaviourTree::Death].onEnter = [this]()
        {
            OnStateEngage<UnitBehaviourTree::Death>();
        };
    unitBehaviourTree[UnitBehaviourTree::Paralysis].enteringCondtion = [this]()
        {
            return referenceParalysis.BeingReferenced();
        };
    unitBehaviourTree[UnitBehaviourTree::Paralysis].onEnter = [this]()
        {
            OnStateEngage<UnitBehaviourTree::Paralysis>();
        };
    unitBehaviourTree[UnitBehaviourTree::Paralysis][UnitBehaviourTree::Knockback].enteringCondtion = [this]()
        {
            return !coroutineKnockBack.expired();
        };
    unitBehaviourTree[UnitBehaviourTree::Paralysis][UnitBehaviourTree::Stun].enteringCondtion = [this]()
        {
            return true;
        };
    unitBehaviourTree[UnitBehaviourTree::Pause].enteringCondtion = [this]()
        {
            return referencePause.BeingReferenced();
        };
    unitBehaviourTree[UnitBehaviourTree::Skill].enteringCondtion = [this]()
        {
            return !coroutineSkill.expired() || (CanProcessOrder<UnitOrderType::Skill>() && pendingSkill.get());
        };
    unitBehaviourTree[UnitBehaviourTree::Skill].onEnter = [this]()
        {
            OnStateEngage<UnitBehaviourTree::Skill>();
        };
    unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::SkillOnGoing].enteringCondtion = [this]()
        {
            return !coroutineSkill.expired();
        };
    unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::SkillOnGoing].onExit = [this]()
        {
            OnStateExit<UnitBehaviourTree::SkillOnGoing>();
        };
    unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::SkillCasting].enteringCondtion = [this]()
        {
            return DistanceTo(pendingSkill.get()->targetPos) < pendingSkill.get()->GetCastRange();
        };
    unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::SkillCasting].onEnter = [this]()
        {
            OnStateEngage<UnitBehaviourTree::SkillCasting>();
        };
    unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::SkillCasting].onExit = [this]()
        {
            OnStateExit<UnitBehaviourTree::SkillCasting>();
        };
    unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::Move].enteringCondtion = [this]()
        {
            return true;
        };
    unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::Move].onEnter = [this]()
        {
            moveDestination = pendingSkill.get()->targetPos;
            OnStateEngage<UnitBehaviourTree::Move>();
        };
    unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::Move].onUpdate = [this]()
        {
            OnStateUpdate<UnitBehaviourTree::Move>();
        };
    unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::Move].onExit = [this]()
        {
            OnStateExit<UnitBehaviourTree::Move>();
        };

    unitBehaviourTree[UnitBehaviourTree::Chasing].enteringCondtion = [this]()
        {
            return ((!pendingTargetUnit.expired() && pendingTargetUnit.lock()->IsAlive()) ||
                (!currentTargetUnit.expired() && currentTargetUnit.lock()->IsAlive())) &&
                CanProcessOrder<UnitOrderType::AttackUnit>();
        };
    unitBehaviourTree[UnitBehaviourTree::Chasing].onEnter = [this]()
        {
            if (!pendingTargetUnit.expired())
            {
                currentTargetUnit = pendingTargetUnit;
                pendingTargetUnit.reset();
            }
            currentOrderType = pendingOrderType;
        };
    unitBehaviourTree[UnitBehaviourTree::Chasing].onUpdate = [this]()
        {
            OnStateEngage<UnitBehaviourTree::Chasing>();
        };
    unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Attack].enteringCondtion = [this]()
        {
            return attackRange.lock()->GetEnemies().contains(currentTargetUnit.lock().get()) || !coroutineAttack.expired();
        };
    unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Attack].onEnter = [this]()
        {
            OnStateEngage<UnitBehaviourTree::Attack>();
        };
    unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Attack].onExit = [this]()
        {
            OnStateExit<UnitBehaviourTree::Attack>();
        };
    unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Attack].onUpdate = [this]()
        {
            OnStateUpdate<UnitBehaviourTree::Attack>();
        };
    unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Move].enteringCondtion = [this]()
        {
            return true;
        };
    unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Move].onEnter = [this]()
        {
            OnStateEngage<UnitBehaviourTree::Move>();
        };
    unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Move].onUpdate = [this]()
        {
            moveDestination = GetAttackPosition(currentTargetUnit);
            OnStateUpdate<UnitBehaviourTree::Move>();
        };
    unitBehaviourTree[UnitBehaviourTree::Hold].enteringCondtion = [this]()
        {
            return CanProcessOrder<UnitOrderType::Hold>();
        };
    unitBehaviourTree[UnitBehaviourTree::Hold].onEnter = [this]()
        {
            currentOrderType = UnitOrderType::Hold;
        };
    unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Attack].enteringCondtion = [this]()
        {
            return !attackRange.lock()->GetEnemies().empty() || !coroutineAttack.expired();
        };
    unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Attack].onEnter = [this]()
        {
            currentTargetUnit = GetClosestEnemy();
            OnStateEngage<UnitBehaviourTree::Attack>();
        };
    unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Attack].onExit = [this]()
        {
            OnStateExit<UnitBehaviourTree::Attack>();
        };
    unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Attack].onUpdate = [this]()
        {
            OnStateUpdate<UnitBehaviourTree::Attack>();
        };
    unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Stop].enteringCondtion = [this]()
        {
            return true;
        };
    unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Stop].onEnter = [this]()
        {
            OnStateEngage<UnitBehaviourTree::Stop>();
        };
    unitBehaviourTree[UnitBehaviourTree::Move].enteringCondtion = [this]()
        {
            return CanProcessOrder<UnitOrderType::Move>();
        };
    unitBehaviourTree[UnitBehaviourTree::Move].onEnter = [this]()
        {
            currentOrderType = pendingOrderType;
            OnStateEngage<UnitBehaviourTree::Move>();
        };
    unitBehaviourTree[UnitBehaviourTree::Move].onUpdate = [this]()
        {
            OnStateUpdate<UnitBehaviourTree::Move>();
        };

    unitBehaviourTree[UnitBehaviourTree::AttackMove].enteringCondtion = [this]()
        {
            //return CanProceedOrder<UnitOrderType::AttackMove>();
            return true;
        };
    unitBehaviourTree[UnitBehaviourTree::AttackMove].onEnter = [this]()
        {
            //currentOrderType = pendingOrderType;
            currentOrderType = UnitOrderType::AttackMove;
        };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Attack].enteringCondtion = [this]()
        {
            return !attackRange.lock()->GetEnemies().empty() || !coroutineAttack.expired();
        };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Attack].onEnter = [this]()
        {
            currentTargetUnit = GetClosestEnemy();
            OnStateEngage<UnitBehaviourTree::Attack>();
        };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Attack].onExit = [this]()
        {
            OnStateExit<UnitBehaviourTree::Attack>();
        };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Attack].onUpdate = [this]()
        {
            OnStateUpdate<UnitBehaviourTree::Attack>();
        };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Move].enteringCondtion = [this]()
        {
            constexpr float epsilon = 0.1f;
            return !acquisitionRange.lock()->GetEnemies().empty() || (attackMoveDestination - GetTransform()->GetWorldPosition()).Magnitude() > epsilon;
        };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Move].onEnter = [this]()
        {
            OnStateEngage<UnitBehaviourTree::Move>();
        };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Move].onUpdate = [this]() {
        currentTargetUnit = GetClosestEnemy();
        moveDestination = currentTargetUnit.expired() ? attackMoveDestination : GetAttackPosition(currentTargetUnit);
        OnStateUpdate<UnitBehaviourTree::Move>();
        };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Stop].enteringCondtion = [this]()
        {
            return true;
        };
    unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Stop].onEnter = [this]()
        {
            OnStateEngage<UnitBehaviourTree::Stop>();
        };
}
Vector3d Unit::GetAttackPosition(std::weak_ptr<Unit> opponent)
{
    auto delta = opponent.lock()->GetTransform()->GetWorldPosition() - GetTransform()->GetWorldPosition();
    return opponent.lock()->GetTransform()->GetWorldPosition() - opponent.lock()->unitTemplateData->pod.collisionSize * delta.Normalized();
}
yunutyEngine::coroutine::Coroutine Unit::AttackCoroutine(std::weak_ptr<Unit> opponent)
{
    auto blockAttack = referenceBlockAttack.Acquire();
    defaultAnimationType = UnitAnimType::Idle;
    PlayAnimation(UnitAnimType::Attack, false);
    co_yield coroutine::WaitForSeconds(unitTemplateData->pod.m_attackPreDelay);
    switch (unitTemplateData->pod.attackType.enumValue)
    {
    case UnitAttackType::MELEE:
        opponent.lock()->Damaged(GetWeakPtr<Unit>(), unitTemplateData->pod.m_autoAttackDamage);
        break;
    case UnitAttackType::MISSILE:
        auto projectile = ProjectilePool::SingleInstance().Borrow(GetWeakPtr<Unit>(), opponent.lock()->GetTransform()->GetWorldPosition());
        break;
    }
    auto blockCommand = referenceBlockPendingOrder.Acquire();
    co_yield coroutine::WaitForSeconds(unitTemplateData->pod.m_attackPostDelay);
    blockCommand.reset();
    StartCoroutine(referenceBlockAttack.AcquireForSecondsCoroutine(unitTemplateData->pod.m_atkCooltime - unitTemplateData->pod.m_attackPostDelay - unitTemplateData->pod.m_attackPreDelay));
    co_return;
}
float Unit::DistanceTo(const Vector3d& target)
{
    return (GetTransform()->GetWorldPosition() - target).Magnitude();
}
const editor::Unit_TemplateData& Unit::GetUnitTemplateData()const
{
    return *unitTemplateData;
};
int Unit::GetTeamIndex() const
{
    return teamIndex;
}
std::weak_ptr<Unit> Unit::GetAttackTarget() const
{
    if (currentTargetUnit.expired())
        return pendingTargetUnit;
    return currentTargetUnit;
}
