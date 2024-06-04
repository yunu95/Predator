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

class UIManager;
class UnitProductor;
class SkillSystem;
class BurnEffect;
class Skill;
class UnitBuff;
class UnitBehaviourTree;
class UnitAcquisitionSphereCollider;
class UnitPool;
class PlayerController;
namespace application
{
    namespace editor
    {
        class UnitData;
        class Unit_TemplateData;
    }
}
class Unit : public Component, public PermanentObservee
{
public:
    // 유닛에게 필요한 모든 필수 구성요소들을 생성해주며 유닛의 초기화를 진행한다. Init은 유닛당 한번만 호출된다.
    void Init(const application::editor::Unit_TemplateData* unitTemplateData);
    // 유닛 데이터의 정보에 맞게 이 유닛을 소환한다.
    void Summon(const application::editor::UnitData* unitData);
    // 유닛의 초기화 구문, 유닛의 체력을 정상상태로 만들며, 버프를 모두 제거하고 상태를 Idle로 만든다.
    void Reset();
    const application::editor::Unit_TemplateData& GetUnitTemplateData()const;
    int GetTeamIndex() const;
    void Relocate(const Vector3d& pos);
    void OrderMove(Vector3d position);
    void OrderAttackMove(Vector3d position);
    void OrderAttack(std::weak_ptr<Unit> opponent);
    void OrderHold();
    template<typename SkillType>
    void OrderSkill(const SkillType& skill);
    template<typename BuffType>
    void ApplyBuff(const BuffType& buff);
    void Damaged(std::weak_ptr<Unit> opponentUnit, float opponentAp);	// 데미지 입었을 경우 추적하는 로직 포함
    void Damaged(float dmg);                            // 추적받지 않는 데미지
    void Heal(float healingPoint);
    void SetUnitCurrentHp(float p_newHp);
    void KnockBack(Vector3d targetPosition, float knockBackDuration);
    void Paralyze(float paralyzeDuration);
    yunutyEngine::coroutine::Coroutine KnockBackCoroutine(Vector3d targetPosition, float knockBackDuration);
    void PlayAnimation(UnitAnimType animType, bool repeat = false);
    void SetDefaultAnimation(UnitAnimType animType);
    void SetDesiredRotation(const Vector3d& facingDirection);
    std::weak_ptr<coroutine::Coroutine> SetRotation(const Vector3d& facingDirection, float rotatingTime);
    std::weak_ptr<coroutine::Coroutine> SetRotation(float facingAngle, float rotatingTime);
    coroutine::Coroutine SettingRotation(float facingAngle, float rotatingTime);
    const UnitBehaviourTree& GetBehaviourTree() const { return unitBehaviourTree; };
    float GetUnitCurrentHp() const;
    // AcquireFactor는 수치에 곱연산이 적용될 부분이며, AcquireDelta는 수치에 덧셈 연산이 적용될 부분이다.
    factor::Multiplier<float>& GetDamageMultiplier() { return multiplierDamage; };
    factor::Adder<float>& GetDamageAdder() { return adderDamage; };
    factor::Multiplier<float>& GetAttackSpeedMultiplier() { return multiplierAttackSpeed; };
    factor::Adder<float>& GetCritAdder() { return adderCrit; };
    virtual void OnContentsPlay() override { }
    virtual void OnContentsStop() override;
    virtual Component* GetComponent() override { return this; }
    virtual void OnEnable() override;
    virtual void OnDisable() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnDestroy() override;
    virtual ~Unit();
    bool IsPlayerUnit() const;
    bool IsInvulenerable() const;
    bool IsAlive()const;
    // 유닛의 행동 트리 상태가 전환될 때
    std::array<DelegateCallback<void()>, UnitBehaviourTree::Keywords::KeywordNum>& OnStateEngageCallback() { return onStateEngage; };
    std::array<DelegateCallback<void()>, UnitBehaviourTree::Keywords::KeywordNum>& OnStateExitCallback() { return onStateExit; };
    string name;
    // 내가 공격할 때
    DelegateCallback<void()> onAttack;
    // 내가 때린 공격이 적에게 맞았을 때, 근거리 공격인 경우라면 onAttack과 호출시점이 같겠으나 원거리 공격인 경우에는 시간차가 있을 수 있다. 
    DelegateCallback<void()> onAttackHit;
    // 내가 피해를 입었을 때
    DelegateCallback<void()> onDamaged;
    // 유닛이 새로 생성될 때
    DelegateCallback<void()> onCreated;
    // 유닛이 회전을 끝냈을 때
    DelegateCallback<void()> onRotationFinish;
    Reference referencePause;
    Reference referenceBlockFollowingNavAgent;
    Reference referenceBlockAnimLoop;
    Reference referenceParalysis;
    Reference referenceBlockPendingOrder;
    Reference referenceBlockRotation;
    Reference referenceInvulnerable;
    Reference referenceBlockAttack;
    Reference referenceDisableNavAgent;

    std::weak_ptr<yunutyEngine::graphics::Animator> GetAnimator() { return animatorComponent; }
private:
    void SetNavObstacleActive(bool active);
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
    yunutyEngine::coroutine::Coroutine AttackCoroutine(std::weak_ptr<Unit> opponent);
    UnitBehaviourTree unitBehaviourTree;
    std::array<DelegateCallback<void()>, UnitBehaviourTree::Keywords::KeywordNum> onStateEngage;
    std::array<DelegateCallback<void()>, UnitBehaviourTree::Keywords::KeywordNum> onStateExit;
    Reference referenceEnableNavObstacle;
    std::shared_ptr<Reference::Guard> enableNavObstacle;
    std::shared_ptr<Reference::Guard> disableNavAgent;
    // 공격범위와 적 포착범위
    std::weak_ptr<UnitAcquisitionSphereCollider> attackRange;
    std::weak_ptr<UnitAcquisitionSphereCollider> acquisitionRange;
    std::weak_ptr<yunutyEngine::graphics::Animator> animatorComponent;
    std::weak_ptr<yunutyEngine::NavigationAgent> navAgentComponent;
    // 유닛들이 가만히 있을 때 장애물로 인식하게 만들기 위함.
    std::weak_ptr<yunutyEngine::NavigationObstacle> navObstacle;
    std::weak_ptr<yunutyEngine::physics::SphereCollider> unitCollider;
    std::unordered_map<UnitBuff::Type, std::shared_ptr<UnitBuff>> buffs;
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
    factor::Multiplier<float> multiplierDamage;
    factor::Multiplier<float> multiplierDamageReceive;
    factor::Adder<float> adderDamage;
    factor::Multiplier<float> multiplierAttackSpeed;
    factor::Adder<float> adderCrit;
    std::weak_ptr<yunutyEngine::coroutine::Coroutine> coroutineKnockBack;
    std::weak_ptr<yunutyEngine::coroutine::Coroutine> coroutineAttack;
    std::weak_ptr<yunutyEngine::coroutine::Coroutine> coroutineSkill;
    yunuGI::IAnimation* defaultAnimation;
    friend UnitBuff;
    friend UnitPool;
    friend PlayerController;
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
void Unit::OrderSkill(const SkillType& skill)
{
    static_assert(std::is_base_of<Skill, SkillType>::value, "SkillType must be derived from Skill");
    pendingSkill = std::make_shared<SkillType>(skill);
    static_cast<Skill*>(pendingSkill.get())->owner = GetWeakPtr<Unit>();
    pendingOrderType = UnitOrderType::Skill;
}
template<typename BuffType>
void Unit::ApplyBuff(const BuffType& buff)
{
    static_assert(std::is_base_of<UnitBuff, BuffType>::value, "BuffType must be derived from UnitBuff");
    buffs[buff.GetBuffType()] = std::make_shared<BuffType>(buff);
    buffs[buff.GetBuffType()]->Init(GetWeakPtr<Unit>());
}
