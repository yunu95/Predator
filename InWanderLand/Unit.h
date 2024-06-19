#pragma once
#include "YunutyEngine.h"
#include "UnitBehaviourTree.h"
#include "Timer.h"
#include <list>
#include "RobinSkillDevelopmentSystem.h"
#include "DummyComponent.h"
#include "ContentsObservee.h"
#include "StatusEffect.h"
#include "LocalTimeEntity.h"
#include "UIElement.h"
#include "DelegateCallback.h"   
#include "TemplateDataList.h"
#include "UnitAnimationType.h"
#include "Unit_TemplateData.h"
#include "UnitOrderType.h"
#include "UnitBuff.h"
#include "Adder.h"
#include "Multiplier.h"
#include "Reference.h"
#include "DamageType.h"
#include "ITacticObject.h"
#include "UnitCollider.h"

class ManagedFBX;
class PassiveSkill;
class UIManager;
class UnitProductor;
class SkillSystem;
class BurnEffect;
class Skill;
class UnitCollider;
class UnitBuff;
class UnitBuffTaunted;
class UnitBehaviourTree;
class UnitAcquisitionSphereCollider;
class UnitPool;
class PlayerController;
class UnitController;
class BossController;
class UnitCapsuleCollider;

namespace wanderUtils
{
    namespace UnitCoroutine
    {
        class ForSecondsFromUnit;
    }
}

namespace application
{
    namespace editor
    {
        class UnitData;
        class Unit_TemplateData;
    }
}
class Unit : public Component, public PermanentObservee, public ITacticObject
{
public:
    static Vector3d FromTo(std::weak_ptr<Unit> from, std::weak_ptr<Unit> to);
    static Vector3d FromTo(Unit* from, Unit* to);
    static float Distance(std::weak_ptr<Unit> a, std::weak_ptr<Unit> b);
    static float DistanceSquare(std::weak_ptr<Unit> a, std::weak_ptr<Unit> b);
    static float Distance(Unit* a, Unit* b);
    static float DistanceSquare(Unit* a, Unit* b);
    static float Distance(Unit* a, const Vector3d& worldPos);
    static float DistanceSquare(Unit* a, const Vector3d& worldPos);
    float Distance(const Vector3d& worldPos);
    float DistanceSquare(const Vector3d& worldPos);
    // 유닛에게 필요한 모든 필수 구성요소들을 생성해주며 유닛의 초기화를 진행한다. Init은 유닛당 한번만 호출된다.
    void Init(const application::editor::Unit_TemplateData* unitTemplateData);
    // 유닛 데이터의 정보에 맞게 이 유닛을 소환한다.
    void Summon(const application::editor::UnitData* unitData);
    void Summon(application::editor::Unit_TemplateData* td, const Vector3d& position, float rotation, bool instant = false);
    void Summon(application::editor::Unit_TemplateData* td, const Vector3d& position, const Quaternion& rotation, bool instant = false);
    // 유닛의 초기화 구문, 유닛의 체력을 정상상태로 만들며, 버프를 모두 제거하고 상태를 Idle로 만든다.
    void Reset();
    const application::editor::Unit_TemplateData& GetUnitTemplateData()const;
    int GetTeamIndex() const;
    std::weak_ptr<Unit> GetAttackTarget() const;
    void Relocate(const Vector3d& pos);
    void OrderMove(Vector3d position);
    void OrderAttackMove(Vector3d position);
    void OrderAttack(std::weak_ptr<Unit> opponent);
    void OrderHold();
    template<typename SkillType>
    void OrderSkill(SkillType&& skill, Vector3d pos);
    template<typename Buff>
    void ApplyBuff(Buff&& buff);
    void EraseBuff(UnitBuffType buffType);
    void Damaged(std::weak_ptr<Unit> opponentUnit, float opponentAp, DamageType damageType = DamageType::Miscellaneous);	// 데미지 입었을 경우 추적하는 로직 포함
    void Damaged(float dmg);                            // 추적받지 않는 데미지
    coroutine::Coroutine DamagedEffectCoroutine(std::weak_ptr<Unit> opponent);
    void Heal(float healingPoint);
    coroutine::Coroutine HealEffectCoroutine();
    void SetCurrentHp(float p_newHp);
    void KnockBack(Vector3d targetPosition, float knockBackDuration);
    /// Unit 의 위치로부터 입력한 위치벡터(월드 좌표계 기준)에 KnockBack 을 수행합니다.
    void KnockBackRelativeVector(Vector3d relativeVector, float knockBackDuration);
    void Paralyze(float paralyzeDuration);
    coroutine::Coroutine ParalyzeEffectCoroutine(float paralyzeDuration);
    yunutyEngine::coroutine::Coroutine KnockbackCoroutine(std::shared_ptr<Reference::Guard> paralysisGuard, Vector3d targetPosition, float knockBackDuration, bool relative = false);
    void PlayAnimation(UnitAnimType animType, Animation::PlayFlag playFlag = Animation::PlayFlag_::Blending);
    void BlendWithDefaultAnimation();
    void SetDefaultAnimation(UnitAnimType animType);
    void SetDesiredRotation(const Vector3d& facingDirection);
    std::weak_ptr<coroutine::Coroutine> SetRotation(const Vector3d& facingDirection, float rotatingTime);
    std::weak_ptr<coroutine::Coroutine> SetRotation(float facingAngle, float rotatingTime);
    coroutine::Coroutine SettingRotation(float facingAngle, float rotatingTime);
    const UnitBehaviourTree& GetBehaviourTree() const { return unitBehaviourTree; };
    float GetUnitCurrentHp() const;
    float GetUnitMaxHp() const;
    // AcquireFactor는 수치에 곱연산이 적용될 부분이며, AcquireDelta는 수치에 덧셈 연산이 적용될 부분이다.
    factor::Adder<float> multiplierDamage;
    factor::Adder<float> multiplierDamageReceive;
    factor::Adder<float> adderAttackDamage;
    // adderAttackSpeed에 담긴 값들은 합연산되어 공격 주기에 1 / (1 + adderAttackSpeed)의 수치로 곱해진다.
    factor::Adder<float> adderAttackSpeed;
    factor::Multiplier<float> multiplierAttackSpeed;
    factor::Adder<float> adderCritChance;
    virtual void OnContentsPlay() override { }
    virtual void OnContentsStop() override;
    virtual Component* GetComponent() override { return this; }
    virtual void OnEnable() override;
    virtual void OnDisable() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnDestroy() override;
    virtual ~Unit();

    virtual void OnPause() override;
    virtual void OnResume() override;

    bool IsPlayerUnit() const;
    bool IsInvulenerable() const;
    bool IsAlive()const;
    bool IsPaused() const { return isPaused; }
    bool IsPreempted() const;
    bool IsTacTicReady() const;
    std::string GetFBXName() const;
    // 콜라이더 캡슐 내부의 무작위 위치를 반환합니다.
    Vector3d GetRandomPositionInsideCapsuleCollider();
    // 유닛의 행동 트리 상태가 전환될 때
    std::array<DelegateCallback<void>, UnitBehaviourTree::Keywords::KeywordNum>& OnStateEngageCallback() { return onStateEngage; };
    std::array<DelegateCallback<void>, UnitBehaviourTree::Keywords::KeywordNum>& OnStateExitCallback() { return onStateExit; };
    string name;
    bool playingBattleAnim{ true };
    // 내가 공격할 때, 매개변수는 내가 공격하는 상대
    DelegateCallback<std::weak_ptr<Unit>> onAttack;
    // 내가 때린 공격이 적에게 맞았을 때, 근거리 공격인 경우라면 onAttack과 호출시점이 같겠으나 원거리 공격인 경우에는 시간차가 있을 수 있다. 
    // 매개변수는 피해를 받은 상대
    DelegateCallback<std::weak_ptr<Unit>> onAttackHit;
    // 내가 피해를 입었을 때, 매개변수는 피해를 준 상대
    DelegateCallback<std::weak_ptr<Unit>> onDamagedFromUnit;
    // 상대가 특정되지 않고 일반적으로 대미지 받은 모든 경우에 호출되는 callback
    DelegateCallback<void> onDamaged;
    // 유닛이 새로 생성될 때
    DelegateCallback<void> onCreated;
    // 유닛이 회전을 끝냈을 때
    DelegateCallback<void> onRotationFinish;
    Reference referencePause;
    Reference referenceBlockFollowingNavAgent;
    Reference referenceBlockAnimLoop;
    Reference referenceParalysis;
    Reference referenceBlockPendingOrder;
    Reference referenceBlockRotation;
    Reference referenceBlindness;
    Reference referenceInvulnerable;
    Reference referenceBlockAttack;
    // NavAgent 객체를 아예 비활성화함.
    Reference referenceDisableNavAgent;
    // NavObstacle 객체를 활성화함.
    Reference referenceEnableNavObstacle;
    // 전술모드에서 명령을 내릴 수 있는지에 대한 Ref
    Reference referenceTactic;
    Reference referenceBlockDeath;

    std::weak_ptr<yunutyEngine::graphics::Animator> GetAnimator() { return animatorComponent; }
private:
    void AddPassiveSkill(std::shared_ptr<PassiveSkill> skill);
    void Summon(application::editor::Unit_TemplateData* templateData);
    void SetIsAlive(bool isAlive);
    void UpdateRotation();
    void InitBehaviorTree();
    template<UnitBehaviourTree::Keywords state>
    void OnStateEngage()
    {
        onStateEngage[state]();
    }
    template<UnitBehaviourTree::Keywords state>
    void OnStateUpdate() {};
    template<UnitBehaviourTree::Keywords state>
    void OnStateExit()
    {
        onStateExit[state]();
    };
    std::weak_ptr<Unit> GetClosestEnemy();
    template<UnitOrderType orderType>
    bool CanProcessOrder();
    // 상대 유닛에 대해 다가가서 때리기 좋은 위치를 반환합니다.
    Vector3d GetAttackPosition(std::weak_ptr<Unit> opponent);
    // 유닛 부활 코루틴
    yunutyEngine::coroutine::Coroutine RevivalCoroutine();
    // 유닛이 새로 생성될때 애니메이션, 번 이펙트와 함께 나오는 코루틴
    yunutyEngine::coroutine::Coroutine BirthCoroutine();
    // 유닛이 죽을 때 애니메이션, 번 이펙트와 함께 사라지고 유닛 풀에 반환되는 코루틴
    yunutyEngine::coroutine::Coroutine DeathCoroutine();
    yunutyEngine::coroutine::Coroutine AttackCoroutine(std::weak_ptr<Unit> opponent);
    yunutyEngine::coroutine::Coroutine MeleeAttackEffectCoroutine(std::weak_ptr<Unit> opponent);
    float DistanceTo(const Vector3d& target);
    void ReturnToPool();
    int liveCountLeft{ 0 };
    std::vector<UnitController*> controllers;
    UnitBehaviourTree unitBehaviourTree;
    std::array<DelegateCallback<void>, UnitBehaviourTree::Keywords::KeywordNum> onStateEngage;
    std::array<DelegateCallback<void>, UnitBehaviourTree::Keywords::KeywordNum> onStateExit;
    std::shared_ptr<PassiveSkill> passiveSkill;
    std::shared_ptr<Reference::Guard> enableNavObstacleByState;
    std::shared_ptr<Reference::Guard> blockFollowingNavAgentByState;
    std::shared_ptr<Reference::Guard> disableNavAgentByState;
    std::shared_ptr<Reference::Guard> invulnerabilityByState;
    // 공격범위와 적 포착범위
    std::weak_ptr<UnitAcquisitionSphereCollider> attackRange;
    std::weak_ptr<UnitAcquisitionSphereCollider> acquisitionRange;
    std::weak_ptr<yunutyEngine::graphics::Animator> animatorComponent;
    std::weak_ptr<yunutyEngine::NavigationAgent> navAgentComponent;
    // 유닛들이 가만히 있을 때 장애물로 인식하게 만들기 위함.
    std::weak_ptr<yunutyEngine::NavigationObstacle> navObstacle;
    std::weak_ptr<UnitCapsuleCollider> unitCollider;
    std::unordered_map<UnitBuffType, std::shared_ptr<UnitBuff>> buffs;
    std::shared_ptr<Skill> onGoingSkill;
    std::shared_ptr<Skill> pendingSkill;
    std::weak_ptr<BurnEffect> burnEffect;
    std::weak_ptr<Unit> currentTargetUnit;					// Attack이나 Chase 때 사용할 적군  오브젝트
    std::weak_ptr<Unit> pendingTargetUnit;					// Attack이나 Chase 때 사용할 적군  오브젝트
    UnitOrderType currentOrderType{ UnitOrderType::Hold };
    UnitOrderType pendingOrderType{ UnitOrderType::Hold };
    const application::editor::Unit_TemplateData* unitTemplateData{ nullptr };
    const application::editor::UnitData* unitData{ nullptr };
    GameObject* skinnedMeshGameObject{ nullptr };
    int teamIndex{ 0 };
    // 유닛의 회전속도는 외부에서 조절할 수 있다. 평소에는 template 데이터의 회전속도와 같다.
    float currentRotationSpeed;
    Vector3d moveDestination;
    Vector3d attackMoveDestination;
    Vector3d lastPosition;
    float currentHitPoint;
    // 유닛의 현재 회전값을 동경각으로 나타냅니다. 유닛은 currentRotation이 0도일때 동쪽(양의 X 방향), 90도일때 북쪽(양의 z 방향)을 향합니다.
    float currentRotation{ 270 };
    // 유닛이 바라봐야 하는 회전값을 동경각으로 나타냅니다. 유닛은 회전속도에 따라 회전값을 desiredRotation과 일치하게 바꿉니다.
    float desiredRotation{ 270 };
    bool isAlive{ false };
    // 개별 유닛의 상태를 나타내는 UI, 보통 체력바라고 보면 된다.
    weak_ptr<UIElement> unitStatusUI;
    // 초상화까지 있는 플레이어측 캐릭터 UI
    std::weak_ptr<UIElement> unitStatusPortraitUI;
    std::weak_ptr<yunutyEngine::coroutine::Coroutine> coroutineBirth;
    std::weak_ptr<yunutyEngine::coroutine::Coroutine> coroutineDeath;
    std::weak_ptr<yunutyEngine::coroutine::Coroutine> coroutineKnockBack;
    std::weak_ptr<yunutyEngine::coroutine::Coroutine> coroutineRevival;
    std::weak_ptr<yunutyEngine::coroutine::Coroutine> coroutineAttack;
    std::weak_ptr<yunutyEngine::coroutine::Coroutine> coroutineAttackEffect;
    std::weak_ptr<yunutyEngine::coroutine::Coroutine> coroutineDamagedEffect;
    std::weak_ptr<yunutyEngine::coroutine::Coroutine> coroutineHealEffect;
    std::weak_ptr<yunutyEngine::coroutine::Coroutine> coroutineSkill;
    UnitAnimType defaultAnimationType;
    bool blendWithDefaultAnimTrigger{ false };
    int navAgentEnableFrameCount{ 0 };
    std::weak_ptr<ManagedFBX> attackVFX = std::weak_ptr<ManagedFBX>();
    std::weak_ptr<ManagedFBX> damagedVFX = std::weak_ptr<ManagedFBX>();
    std::weak_ptr<ManagedFBX> healVFX = std::weak_ptr<ManagedFBX>();
	std::weak_ptr<ManagedFBX> paralysisVFX = std::weak_ptr<ManagedFBX>();

    bool isPaused = false;
    float localTimeScale = 1.0f;

    friend UnitBuff;
    friend UnitPool;
    friend PlayerController;
    friend BossController;
    friend UnitBuffTaunted;
    friend wanderUtils::UnitCoroutine::ForSecondsFromUnit;
};
template<UnitOrderType orderType>
bool Unit::CanProcessOrder()
{
    {
        return (currentOrderType == orderType && pendingOrderType == currentOrderType) ||
            (!referenceBlockPendingOrder.BeingReferenced() && pendingOrderType == orderType);
    };
}

template<typename SkillType>
void Unit::OrderSkill(SkillType&& skill, Vector3d pos)
{
    static_assert(std::is_base_of<Skill, SkillType>::value, "SkillType must be derived from Skill");
    pendingSkill = std::make_shared<SkillType>(std::move(skill));
    static_cast<Skill*>(pendingSkill.get())->owner = GetWeakPtr<Unit>();
    static_cast<Skill*>(pendingSkill.get())->selfWeakPtr = std::dynamic_pointer_cast<Skill>(pendingSkill);
    static_cast<Skill*>(pendingSkill.get())->targetPos = pos;
    pendingOrderType = UnitOrderType::Skill;
}

template<typename Buff>
void Unit::ApplyBuff(Buff&& buff)
{
    static_assert(std::is_base_of<UnitBuff, Buff>::value, "BuffType must be derived from UnitBuff");
    if (buffs.contains(buff.GetBuffType()))
    {
        buffs[buff.GetBuffType()]->OnOverlap(std::move(buff));
    }
    else
    {
        buffs[buff.GetBuffType()] = std::make_shared<Buff>(std::move(buff));
        buffs[buff.GetBuffType()]->Init(GetWeakPtr<Unit>());
        buffs[buff.GetBuffType()]->OnStart();
    }
}
