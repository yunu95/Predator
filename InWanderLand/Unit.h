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

class UIManager;
class UnitProductor;
class SkillSystem;
class BurnEffect;
class CursorDetector;
enum class SkillPreviewMesh;

/// <summary>
/// 유닛들이 공유하는 멤버.
/// </summary>
class Unit : public Component, public ContentsObservee, public LocalTimeEntity
{
public:
    // 사용 시 주의점 : 마지막에는 Death와 StateEnd가 순서대로 들어가 있을 것!
    enum class UnitState
    {
        Idle,
        Move,
        Chase,
        Attack,
        AttackMove,
        Skill,
        Paralysis,
        Death,
        Resurrect,
        OffsetMove,
        WaveStart,
        WaveMotion,
        StateEnd
    };

    enum class UnitType
    {
        Warrior = 1,
        Magician = 2,
        Healer = 3,
        AllPlayers = 4,
        MeleeEnemy = 5,
        RangedEnemy = 6,
        Boss = 7,
        EnemySpawnGate = 8,
        SpikeTrap = 9,
        ChessTrap = 10,
        TriggeredTrap = 11
    };

    enum class UnitSide
    {
        Player,
        Enemy
    };

    enum class SkillEnum
    {
        Q,
        W,
        BossSkillOne,
        BossSkillTwo,
        BossSkillThree,
        BossSkillFour
    };

    struct BaseUnitAnimationStruct
    {
        yunuGI::IAnimation* m_idleAnimation;
        yunuGI::IAnimation* m_walkAnimation;
        yunuGI::IAnimation* m_attackAnimation;
        yunuGI::IAnimation* m_paralysisAnimation;
        yunuGI::IAnimation* m_deathAnimation;
        yunuGI::IAnimation* m_battleEngageAnimation;
        yunuGI::IAnimation* m_skillOneAnimation;
        yunuGI::IAnimation* m_skillTwoAnimation;
        yunuGI::IAnimation* m_skillThreeAnimation;
        yunuGI::IAnimation* m_skillFourAnimation;
    };

    TimerComponent* knockBackTimer;
    Dotween* dotween;
    yunuGI::IAnimation* m_currentAnimation{ nullptr };
    yunutyEngine::graphics::Animator* m_animatorComponent;
    NavigationAgent* m_navAgentComponent;
    BurnEffect* m_burnEffect;
    PlayerSkillSystem* m_playerSkillSystem;

    Unit* m_currentTargetUnit;					// Attack이나 Chase 때 사용할 적군  오브젝트
    //Vector3d startPosition;
private:
    FSM<UnitState> unitFSM{ UnitState::Idle };
    SkillSystem* m_skillSystemComponent;
    UnitType m_unitType;
    UnitSide m_unitSide;

    std::string m_fbxName;
    float m_maxHealthPoint;
    float m_currentHealthPoint;
    float m_autoAttackDamage;
    float m_beforeBlindDamage;
    float m_initialAutoAttackDamage;
    float m_maxAutoAttackDamage = 10.0f;
    float m_minAttackDelay = 0.3f;

    float m_finalAttackDamage;
    float m_finalHitDamage;

    int m_randcriticalHitNumber;

    float m_defensePoint;
    float m_dodgeProbability;
    float m_criticalDamageDecreaseMultiplier;
    float m_criticalHitMultiplier;

    int m_criticalHitProbability;

    float m_speed;
    float m_bulletSpeed;

    float chaseUpdateDelay;
    // 유닛의 현재 회전값을 동경각으로 나타냅니다. 유닛은 currentRotation이 0도일때 동쪽(양의 X 방향), 90도일때 북쪽(양의 z 방향)을 향합니다.
    float currentRotation{ 270 };
    // 유닛이 바라봐야 하는 회전값을 동경각으로 나타냅니다. 유닛은 회전속도에 따라 회전값을 desiredRotation과 일치하게 바꿉니다.
    float desiredRotation{ 270 };
    float rotationSpeed{ 500 };
    float rotateTime = 0.2f;

    // 지금 수행중인 명령
    UnitState currentOrder = UnitState::Idle;

    float m_idDistance;
    float m_atkDistance;

    float idleToChaseDelay = 1.5f;
    float idleElapsed;

    float moveFunctionElapsed;
    float moveFunctionCallDelay = 0.1f;

    float chaseFunctionElapsed;
    float chaseFunctionCallDelay = 0.1f;

    float attackStartElapsed = 0.0f;

    float attackStartDelay = 0.3f;
    float attackFunctionElapsed;
    float attackFunctionCallDelay;
    float increasedAttackSpeed{ 1.0f };
    int attackTimingFrame;
    float m_attackOffset;

    float deathFunctionElapsed;
    float deathAnimationDelay = 1.5f;

    int m_resurrectingMaxCount{ 0 };
    int m_currentResurrectingCount{ 0 };
    float m_resurrectingDuration{ 5.0f };

    float skillFunctionStartElapsed;
    float m_currentSelectedSkillEngageDelay;
    int m_selectedSkillTimingFrame;
    yunuGI::IAnimation* m_currentSkillAnimation;
    std::unordered_map<SkillEnum, float> m_skillDurationMap;
    std::unordered_map<SkillEnum, yunuGI::IAnimation*> m_skillAnimationMap;
    std::unordered_map<SkillEnum, int> m_skillTimingFrameMap;
    //float qSkillFunctionStartedElapsed;
    //float qSkillStartDelay = 1.0f;
    bool isJustHitByQSkill = false;

    float m_battleEngageMotionElapsed{ 0.0f };
    float m_battleEngageDuration{ 5.0f };

    bool isAttackMoving;

    UnitType playerSerialNumber;

    std::set<Unit*> m_opponentObjectSet;		// idradius에 닿는 모든 상대 유닛을 담아줍니다.
    std::set<Unit*> m_recognizedThisSet;		// 현재 이 유닛을 적군으로 인식한 다른 유닛들의 리스트.

    std::set<Unit*> m_attackingThisUnitSet;		// 현재 이 유닛을 공격하고 있는 유닛들
    int m_maxAggroNumber;

    Unit* m_previousTargetUnit;
    Unit* tauntingThisUnit;						// 현재 this 유닛이 도발당한 주체.
    Vector3d m_previousMovePosition;
    Vector3d m_currentMovePosition;				// 현재 상대의 위치
    Vector3d m_waveStartPosition;				// 웨이브 시작 위치

    yunutyEngine::graphics::StaticMeshRenderer* m_staticMeshRenderer;

    SkillPreviewMesh m_qSkillPreviewType;
    SkillPreviewMesh m_wSkillPreviewType;

    Unit* m_followingTargetUnit;
    float m_followEngageDinstance{ 4.0f };			// 이 수치만큼 거리가 벌어지면 따라간다.
    float m_stopFollowDinstance{ 2.0f };			// 이 수치만큼 거리가 좁혀지면 멈춘다.
    bool isFollowing{ false };

public:
    bool isPermittedToTacticAction{ false };

private:
    // 주로 개별 유닛의 상태를 나타내는 UI
    weak_ptr<UIElement> unitStatusUI;
    // 초상화까지 있는 플레이어측 캐릭터 UI
    UIElement* unitStatusPortraitUI{ nullptr };
    /// 유닛이 속해있는 field
    NavigationField* m_unitNavField;

    /// 현재 스킬 활성화 시 적용되는 스킬 적용 위치
    Vector3d m_skillPosition;
    SkillEnum m_currentSelectedSkill;

private:
    void IdleEngage();
    void MoveEngage();
    void OffsetMoveEngage();
    void AttackMoveEngage();
    void AttackEngage();
    void ChaseEngage();
    void SkillEngage();
    void ParalysisEngage();
    void DeathEngage();
    void WaveStartEngage();
    void WaveMotionEngage();
    void ResurrectEngage();

    void IdleUpdate();
    void MoveUpdate();
    void OffsetMoveUpdate();
    void AttackMoveUpdate();
    void ChaseUpdate();
    void AttackUpdate();
    void SkillUpdate();
    void DeathUpdate();
    void WaveStartUpdate();
    void WaveMotionUpdate();
    void ResurrectUpdate();

    void ChangeAnimation(yunuGI::IAnimation* p_anim);
    void CheckCurrentAnimation(yunuGI::IAnimation* currentStateAnimation);

    void ReportUnitDeath();												// this 유닛이 죽었다는 정보를 전달
    void IdentifiedOpponentDeath(Unit* p_unit);		// 상대 유닛이 죽었을 경우 처리할 내용을 담은 함수

    void DetermineHitDamage(float p_onceCalculatedDmg);					// 피격유닛이 받는 최종 데미지 계산

    void RotateUnit(Vector3d endPosition);

    void StopAnimation();

    void RegisterSkillWithAnimation(SkillEnum p_enum);

public:
    Vector3d m_currentSkillPosition;

    BaseUnitAnimationStruct unitAnimations;
    float animationLerpDuration = 0.1f;
    float animationTransitionSpeed = 1.0f;
    bool isAttackAnimationOperating{ false };

    virtual void OnEnable() override;
    virtual void Start() override;
    virtual void Update() override;
    virtual void OnDestroy() override;

    virtual void PlayFunction() override;
    virtual void StopFunction() override;

    void StopMove();
    UnitType GetUnitType() const;
    UnitSide GetUnitSide() const;
    void SetUnitType(UnitType type);
    void SetUnitSide(UnitSide side);
    void SetUnitMaxHp(int p_Hp);
    void SetUnitAp(int p_Ap);
    void SetIdRadius(float radius);
    void SetAtkRadius(float radius);
    void SetUnitSpeed(float speed);
    void SetAttackDelay(float p_delay);
    void SetAttackTimingFrame(int p_frame);
    void SetPlayerSerialNumber(UnitType serialNum);
    void SetSkillPreviewType(SkillPreviewMesh p_qskill, SkillPreviewMesh p_wskill);
    void SetMaxAggroNumber(int p_num);
    void SetFbxName(std::string p_string);
    void SetAttackOffset(float p_offset);
    void RegisterSkillDuration(SkillEnum p_skillEnum, float p_duration);
    void RegisterSkillAnimation(SkillEnum p_skillEnum, yunuGI::IAnimation* p_anim);
    void RegisterSkillTimingFrame(SkillEnum p_skillEnum, int p_frame);

    void SetCurrentOrderMove();
    void SetCurrentOrderAttackMove();

    float GetAttackOffset() const;

    void OrderMove(Vector3d position);
    void OrderAttackMove(Vector3d position);
    void OrderAttackMove(Vector3d position, Unit* p_selectedUnit);
    void OrderSkill(SkillEnum p_skillNum, Vector3d position);
    void OrderSkill(SkillEnum p_skillNum);

    void ExecuteSkillAction(Vector3d p_pos, SkillEnum p_skillNum);

    void DetermineCurrentTargetObject();
    void AddToOpponentObjectList(Unit* p_unit);
    void DeleteFromOpponentObjectList(Unit* p_unit);

    void SetNavField(NavigationField* p_navField);
    NavigationField* GetNavField() const;

    void MakeUnitPushedState(bool p_isCrushed);
    void MakeUnitParalysisState();
    void SetUnitStateIdle();

    UIElement* GetBarBuffIcon(StatusEffect::StatusEffectEnum uiEnumID);
    UIElement* GetPortraitBuffIcon(StatusEffect::StatusEffectEnum uiEnumID);
    void ReportStatusEffectApplied(StatusEffect::StatusEffectEnum p_effectType);
    void ReportStatusEffectEnded(StatusEffect::StatusEffectEnum p_effectType);

    void PermitTacticAction();

    bool GetJustCrushedState() const;
    bool IsUnitDead() const;

    UnitType GetPlayerSerialNumber() const;
    UnitState GetCurrentUnitState() const;
    SkillPreviewMesh GetSkillPreviewType(Unit::SkillEnum p_currentSkillType) const;
    std::string GetUnitFbxName() const;

    float DetermineAttackDamage(float p_damage);			// 공격유닛이 피격유닛에게 전달하는 데미지 계산.

    void SetStaticMeshComponent(yunutyEngine::graphics::StaticMeshRenderer* p_stcMesh);

    void ChangeCurrentOpponentUnitForced(Unit* p_unit);
    void DeleteTauntingUnit();

    void SetUnitStateToDeath();
    void SetUnitStateToSkill();

    void SetCurrentMovePosition(Vector3d p_pos);
    void SetWaveStartPosition(Vector3d p_pos);

    void PushMoveFunctionToTacticQueue(Vector3d p_pos);
    void PushAttackMoveFunctionToTacticQueue(Vector3d p_pos);
    void PushAttackMoveFunctionToTacticQueue(Vector3d p_pos, Unit* p_selectedUnit);
    void PushSkillFunctionToTacticQueue(SkillEnum p_skillNum, Vector3d p_pos);
    void ReportTacticModeEngaged();

    bool IsTacticModeQueueEmpty() const;

    void ChangeUnitStatRandomly();

    void SetRessurectMaxCount(int p_cnt);

public:
    ~Unit();
    int GetUnitDamage() const;
    void Damaged(Unit* opponentUnit, float opponentAp);	// 데미지 입었을 경우 추적하는 로직 포함
    void Damaged(float dmg);										// 추적받지 않는 데미지
    void Heal(float healingPoint);

    void SetUnitCurrentHp(float p_newHp);
    float GetUnitCurrentHp() const;

    void IncreaseAttackPower(float p_attackPowerIncrease);
    void IncreaseAttackSpeed(float p_attackSpeedIncrease);

    void SetUnitDamageToZero(bool p_bool);
    void MultipleUnitSpeed(float p_mul);

    void ResetUnitMembers();

    void SetUnitLocalTimeScale(float p_scale);

    void EnemyActionOnTacticModeEngaged();
    void EnemyActionOnTacticModeEnded();

    void SetCurrentAnimationSpeed(float p_speed);

    bool IsAllExtraPlayerUnitDead();
    bool CheckEnemyStoppedByTacticMode() const;


    std::function<void()> returnToPoolFunction{ nullptr };
    std::function<void()> deathEngageFunction{ nullptr };
    DummyComponent* m_dummyCom;
    int stageNumber;

    std::vector<std::function<void()>> OnCreated;
    std::vector<std::function<void()>> OnDeath;
    bool isJustCreated{ false };

    std::queue<std::function<void()>> m_tacticModeQueue;

    Vector3d m_currentBelongingWavePosition;
    CursorDetector* m_cursorDetectorComponent;

    friend RobinSkillDevelopmentSystem;
    friend UnitProductor;
    friend UIManager;
};

