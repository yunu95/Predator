#pragma once
#include "YunutyEngine.h"
#include "UnitBehaviourTree.h"
#include "Timer.h"
#include "Dotween.h"
#include <list>
#include "RobinSkillDevelopmentSystem.h"
#include "DummyComponent.h"
#include "ContentsObservee.h"
#include "StatusEffect.h"
#include "LocalTimeEntity.h"
#include "UIElement.h"
#include "Delegate.h"
#include "TemplateDataList.h"
#include "UnitState.h"
#include "UnitAnimationType.h"
#include "Unit_TemplateData.h"
#include "UnitOrderType.h"

class UIManager;
class UnitProductor;
class SkillSystem;
class BurnEffect;
class Skill;

class Unit : public Component, public ContentsObservee, public LocalTimeEntity
{
public:
    struct Reference { };
    // 유닛에게 필요한 모든 필수 구성요소들을 생성해주며 유닛의 초기화를 진행한다.
    void Init(const application::editor::Unit_TemplateData* GetUnitTemplateData);
    // 유닛의 초기화 구문, 유닛의 체력을 정상상태로 만들며, 버프를 모두 제거하고 상태를 Idle로 만든다.
    void Reset();
    // 유닛이 생성될 때 실행되는 동작. 함수 내부에서 Reset이 호출된다.

    void Birth();
    const application::editor::Unit_TemplateData& GetUnitTemplateData()const;
    int GetTeamIndex() const;
    UIElement* GetBarBuffIcon(StatusEffect::StatusEffectEnum uiEnumID);
    UIElement* GetPortraitBuffIcon(StatusEffect::StatusEffectEnum uiEnumID);
    UnitState GetCurrentUnitState() const;
    virtual Component* GetComponent() override { return this; }
    float GetUnitCurrentHp() const;
    std::shared_ptr<Reference> AcquirePauseReference();
    // AcquireFactor는 수치에 곱연산이 적용될 부분이며, AcquireDelta는 수치에 덧셈 연산이 적용될 부분이다.
    std::shared_ptr<float> AcquireDamageFactor(float val);
    std::shared_ptr<float> AcquireDamageDelta(float val);
    std::shared_ptr<float> AcquireAttackSpeedFactor(float val);
    std::shared_ptr<float> AcquireCritFactor(float val);
    void StopMove();
    void OrderMove(Vector3d position);
    void OrderAttackMove(Vector3d position);
    void OrderAttackMove(Vector3d position, Unit* opponent);
    void ApplySkill(const Skill& skill);
    void ApplyKnockback();
    void ApplyParalysis();
    void ReportStatusEffectApplied(StatusEffect::StatusEffectEnum p_effectType);
    void ReportStatusEffectEnded(StatusEffect::StatusEffectEnum p_effectType);
    virtual ~Unit();
    void Damaged(std::weak_ptr<Unit> opponentUnit, float opponentAp);	// 데미지 입었을 경우 추적하는 로직 포함
    void Damaged(float dmg);                            // 추적받지 않는 데미지
    void Heal(float healingPoint);
    void SetUnitCurrentHp(float p_newHp);
    void KnockBackUnit(Vector3d targetPosition, float knockBackDuration);
    void PlayAnimation(UnitAnimType animType, bool repeat = false);
    void RotateUnit(Vector3d endPosition);
    void SetDesiredRotation();
    void UpdateRotation();
    virtual void OnEnable() override;
    virtual void OnDisable() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnDestroy() override;
    virtual void OnTransformUpdate() override;
    // 내가 공격할 때
    Delegate onAttack;
    // 내가 때린 공격이 적에게 맞았을 때, 근거리 공격인 경우라면 onAttack과 호출시점이 같겠으나 원거리 공격인 경우에는 시간차가 있을 수 있다. 
    Delegate onAttackHit;
    // 내가 피해를 입었을 때
    Delegate onDamaged;
    // 유닛이 새로 생성될 때
    Delegate onCreated;
    // 유닛이 사망할 때
    Delegate onDeath;
private:
    void InitBehaviorTree();
    template<std::weak_ptr<Reference> Unit::* referenceWeakptr>
    std::shared_ptr<Reference> AcquireReference()
    {
        if ((this->*referenceWeakptr).expired()) this->*referenceWeakptr = std::make_shared<Reference>();
        return this->*referenceWeakptr;
    };
    coroutine::Coroutine Paralyze();
    coroutine::Coroutine KnockBack();
    template<UnitBehaviourTree::Keywords state>
    void OnStateEngage() {};
    template<UnitBehaviourTree::Keywords state>
    void OnStateUpdate() {};
    template<UnitBehaviourTree::Keywords state>
    void OnStateExit() {};
    TimerComponent* knockBackTimer;
    std::vector<std::weak_ptr<float>> damageFactors;
    std::vector<std::weak_ptr<float>> damageDelta;
    std::vector<std::weak_ptr<float>> attackSpeedFactors;
    std::vector<std::weak_ptr<float>> critDeltas;
    UnitBehaviourTree unitBehaviourTree;
    std::weak_ptr<Reference> pauseReference;
    std::weak_ptr<Reference> stopFollowingNavAgentReference;
    std::weak_ptr<Reference> blockPendingOrderReference;
    std::weak_ptr<Reference> blockRotationReference;
    Vector3d knockBackStartPoint;
    std::weak_ptr<yunutyEngine::graphics::Animator> m_animatorComponent;
    std::weak_ptr<yunutyEngine::NavigationAgent> m_navAgentComponent;
    // 유닛들이 가만히 있을 때 장애물로 인식하게 만들기 위함.
    std::weak_ptr<yunutyEngine::NavigationObstacle> m_navObstacle;
    std::weak_ptr<BurnEffect> m_burnEffect;
    std::weak_ptr<Unit> m_currentTargetUnit;					// Attack이나 Chase 때 사용할 적군  오브젝트
    UnitOrderType currentOrderType{ UnitOrderType::Hold };
    UnitOrderType pendingOrderType{ UnitOrderType::Hold };
    const application::editor::Unit_TemplateData* unitTemplateData{ nullptr };
    int teamIndex{ 0 };
    Vector3d currentMoveDestination;
    float m_currentHitPoint;
    // 유닛의 현재 회전값을 동경각으로 나타냅니다. 유닛은 currentRotation이 0도일때 동쪽(양의 X 방향), 90도일때 북쪽(양의 z 방향)을 향합니다.
    float currentRotation{ 270 };
    // 유닛이 바라봐야 하는 회전값을 동경각으로 나타냅니다. 유닛은 회전속도에 따라 회전값을 desiredRotation과 일치하게 바꿉니다.
    float desiredRotation{ 270 };
    // 개별 유닛의 상태를 나타내는 UI, 보통 체력바라고 보면 된다.
    weak_ptr<UIElement> unitStatusUI;
    // 초상화까지 있는 플레이어측 캐릭터 UI
    UIElement* unitStatusPortraitUI{ nullptr };
    bool isFollowingNavAgent{ true };
};

