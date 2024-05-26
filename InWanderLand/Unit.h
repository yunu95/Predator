#pragma once
#include "YunutyEngine.h"
#include "FSM.h"
#include "Timer.h"
#include "Dotween.h"
#include <list>
#include "RobinSkillDevelopmentSystem.h"
#include "DummyComponent.h"
#include "ContentsObservee.h"
#include "StatusEffect.h"
#include "LocalTimeEntity.h"
#include "UIElement.h"
#include "VolatileCallbacks.h"
#include "TemplateDataList.h"
#include "UnitState.h"
#include "UnitAnimationType.h"

class UIManager;
class UnitProductor;
class SkillSystem;
class BurnEffect;
class Skill;

/// <summary>
/// 유닛들이 공유하는 멤버.
/// </summary>
class Unit : public Component, public ContentsObservee, public LocalTimeEntity
{
public:
    struct PauseReference { };
    const editor::Unit_TemplateData& GetUnitTemplateData();
    int GetTeamIndex() const;
    UIElement* GetBarBuffIcon(StatusEffect::StatusEffectEnum uiEnumID);
    UIElement* GetPortraitBuffIcon(StatusEffect::StatusEffectEnum uiEnumID);
    UnitState GetCurrentUnitState() const;
    virtual Component* GetComponent() override { return this; }
    float GetUnitCurrentHp() const;
    std::shared_ptr<PauseReference> AcquirePauseReference();
    // AcquireFactor는 수치에 곱연산이 적용될 부분이며, AcquireDelta는 수치에 덧셈 연산이 적용될 부분이다.
    std::shared_ptr<float> AcquireDamageFactor(float val);
    std::shared_ptr<float> AcquireDamageDelta(float val);
    std::shared_ptr<float> AcquireAttackSpeedFactor(float val);
    std::shared_ptr<float> AcquireCritFactor(float val);
    void StopMove();
    void OrderMove(Vector3d position);
    void OrderAttackMove(Vector3d position);
    void OrderAttackMove(Vector3d position, Unit* p_selectedUnit);
    void ApplySkill(const Skill& skill);
    void ApplyKnockback();
    void ApplyParalysis();
    void ReportStatusEffectApplied(StatusEffect::StatusEffectEnum p_effectType);
    void ReportStatusEffectEnded(StatusEffect::StatusEffectEnum p_effectType);
    virtual ~Unit();
    void Damaged(Unit* opponentUnit, float opponentAp);	// 데미지 입었을 경우 추적하는 로직 포함
    void Damaged(float dmg);                            // 추적받지 않는 데미지
    void Heal(float healingPoint);
    void SetUnitCurrentHp(float p_newHp);
    void KnockBackUnit(Vector3d targetPosition, float knockBackDuration);
    template<UnitState state>
    void OnStateEngage() {};
    template<UnitState state>
    void OnStateUpdate() {};
    template<UnitState state>
    void OnStateExit() {};
    void PlayAnimation(UnitAnimType animType);
    void RotateUnit(Vector3d endPosition);
    virtual void OnEnable() override;
    virtual void OnDisable() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnDestroy() override;
    virtual void OnTransformUpdate() override;
    VolatileCallbacks onCreated;
    VolatileCallbacks onDeath;
private:
    coroutine::Coroutine Paralyze();
    coroutine::Coroutine KnockBack();
    TimerComponent* knockBackTimer;
    std::vector<std::weak_ptr<float>> damageFactors;
    std::vector<std::weak_ptr<float>> damageDelta;
    std::vector<std::weak_ptr<float>> attackSpeedFactors;
    std::vector<std::weak_ptr<float>> critDeltas;
    std::weak_ptr<PauseReference> pauseReference;
    Vector3d knockBackStartPoint;
    std::weak_ptr<yunutyEngine::graphics::Animator> m_animatorComponent;
    std::weak_ptr<yunutyEngine::NavigationAgent> m_navAgentComponent;
    // 유닛들이 가만히 있을 때 장애물로 인식하게 만들기 위함.
    std::weak_ptr<yunutyEngine::NavigationObstacle> m_navObstacle;
    std::weak_ptr<BurnEffect> m_burnEffect;
    std::weak_ptr<Unit> m_currentTargetUnit;					// Attack이나 Chase 때 사용할 적군  오브젝트
    const editor::Unit_TemplateData* unitTemplateData{ nullptr };
    FSM<UnitState> unitFSM{ UnitState::Stop };
    int teamIndex{ 0 };
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
