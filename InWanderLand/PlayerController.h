#pragma once
#pragma once
#include "YunutyEngine.h"
#include "SkillUpgradeType.h"
#include <map>
#include "UnitController.h"
#include "UIManager.h"
#include "DelegateCallback.h"   
#include "PlayerCharacterType.h"
#include "SkillType.h"
#include "UnitAcquisitionBoxCollider.h"
#include "EnqueErrorType.h"
#include "ProgressTracker.h"

// 사용자 입력을 받고 플레이어 유닛들의 행동을 관리하는 클래스
// 짬통 중의 짬통이다.
class RTSCam;
class PlayerUnit;
class IInteractableComponent;
class UnitAcquisitionSphereCollider;
namespace application
{
    class Action_BlockSkillCancel;
    class Action_BlockSkillSelection;
    class Action_BlockPlayerSwitch;
    namespace editor
    {
        class RegionData;
    }
}

// 유닛들이 공유하는 마나와 쿨타임을 관리한다, 스킬들의 업그레이드 상태를 저장하고 관리한다.
// 콤보 목표를 지정하고 달성시 스킬 포인트를 얻는것까지 관여한다.
// GameManager의 역할 중 전투 시작과 전투 끝에 포즈를 잡는 것, 비전투 상태를 관리하는 것, 콤보를 출력하는 역할도 모두
// 이 클래스가 담당한다.
// 전술 모드에 진입하는 것도 이 클래스에서 진입한다.
class PlayerController : public SingletonComponent<PlayerController>, public UnitController, public application::ProgressTracker
{
public:
    struct State
    {
        enum Enum
        {
            None,
            // 전투중
            Battle,
            // 평화상태, 애들이 선두 유닛을 졸졸 따라다님.
            Peace,
            // 전술모드
            Tactic,
            // 시네마틱 상태, 사용자의 모든 입력이 막히고 컨트롤러가 유닛들에 일절 개입하지 않는다.
            Cinematic,
        };
    };

    virtual void RegisterUnit(std::weak_ptr<Unit> unit)override;
    virtual void UnRegisterUnit(std::weak_ptr<Unit> unit) override {};

    void SelectPlayerUnit(PlayerCharacterType::Enum charType);
    void SelectSkill(SkillType::Enum skillType);
    State::Enum GetState() const { return state; }

    const std::array<std::weak_ptr<Unit>, (int)PlayerCharacterType::Num>& GetPlayers() const { return characters; }
    std::array<float, (int)PlayerCharacterType::Num> GetAggroProportions()const;
    void Reset();
    void SetCameraOffset();
    void SetComboObjectives(const std::array<int, 3>& targetCombos);
    void AddCombo();
    void SetSkillUpgradeTarget(UIEnumID skillUpgradeUITarget);
    bool IsSkillUpgraded(SkillUpgradeType::Enum id);
    bool IsSkillUpgraded(UIEnumID skillUpgradeUITarget);
    void UpgradeSkill();
    void SyncSkillUpgradesWithUI();
    void SetSkillPoints(int points);
    void SetZoomFactor(float zoomFactor);
    int GetSkillPoints();
    float GetZoomFactor();
    void IncrementSkillPoint();
    void UnconstrainCamUpdateDirection();
    void ConstrainCamUpdateDirection(const Vector3d& direction);
    void LockCamInRegion(const application::editor::RegionData* camLockRegion);
    void UnlockCamFromRegion();
    bool CanUnitSelectSkill(std::weak_ptr<Unit> unit);
    float GetMana();
    float GetManaNormalized();
    Vector3d GetCamPivotPoint();
    void SetManaFull();
    void SetMana(float mana);
    void SetPendingManaCost(float manaCost);
    void TryTogglingTacticMode();
    Unit* GetUnitOnCursor();
    virtual void OnPause() override;
    virtual void OnResume() override;

    Vector3d GetCamOffsetNorm() const;

    static constexpr int playerTeamIndex = 1;
    static const std::unordered_map<UIEnumID, SkillUpgradeType::Enum> skillUpgradeByUI;
    DelegateCallback<void> onSkillSelect[(int)SkillType::SKILL_NUM];
    DelegateCallback<void> onSkillActivate[(int)SkillType::SKILL_NUM];
    DelegateCallback<void> onSkillTargeted[(int)SkillType::SKILL_NUM];
    DelegateCallback<void> onSkillExpiration[(int)SkillType::SKILL_NUM];
    factor::Multiplier<float> camZoomMultiplier;

    // 시네마틱 모드가 되면 아래의 레퍼런스를 사용한다.
    Reference referenceCinematic;

    void RequestStateFromAction(State::Enum newState);
    void ApplyBeforeEngageSkillCoolTime();
    void SetTacticCamera(GameObject* cam);

    // Game 을 재시작하게 될 경우,
    // 초기화 세팅을 하기 위한 함수입니다.
    virtual void ProgressInitialize();

    // Game 진행 도중에 호출되어 현재의 상태를 저장하여
    // 추후에 Recovery 에서 사용할 수 있도록 개별적으로 적합한
    // Save Data 를 생성하는 함수입니다.
    virtual void CurrentProgressSave();

    // Game 진행 도중에 호출되어 저장되었던 상태를 통해 Load 하는 함수입니다.
    // Callback 지원을 위해 DoRecoveryCallbacks 함수를 호출해야합니다.
    virtual void Recovery();

    // 스킬 업그레이드와 관련된 부분
    // 어떤 스킬을 업그레이드 할 것인지 미리 정한다. 미리 지정만 하는 것이지 바로 업그레이드까지 직행하는 것은 아니다.
    std::unordered_set<PlaytimeWave*> finishedWaves;
private:
    void SetState(State::Enum newState);
    virtual Component* GetComponent() override { return this; }
    virtual void Start() override;
    virtual void OnContentsPlay() override;
    virtual void OnContentsStop() override;
    virtual void Update() override;

    void HandleByState();
    // 사용자 입력을 받기 위해 매 프레임 불린다.
    void HandleInput();
    // PlayerController의 State를 매 프레임 관리한다.
    void HandleState();
    // 카메라의 상태를 매 프레임마다 업데이트시켜주는 함수
    void HandleCamera();
    // 스킬들의 프리뷰 이미지를 매 프레임마다 업데이트시켜주는 함수
    void HandleSkillPreview();
    void HandleSkillCooltime();
    void HandleManaRegen();
    void HandleMouseCursorAndHover();
    void HandleUnitPickingCollider();
    void HandleComboState();
    void HandlePlayerConstrainingRegion();
    void HandlePlayerOutOfCamUI();
    void OnPlayerChracterDead(std::weak_ptr<Unit> unit);
    void OnPlayerChracterAllDead();
    // character가 NONE일 경우 알아서 현재 선택된 스킬로 귀결된다.
    void OnLeftClick();
    void OnRightClick();
    void UnselectUnit();
    void SelectUnit(std::weak_ptr<Unit> unit);
    void OrderMove(Vector3d position);
    void OrderAttackMove(Vector3d position);
    void OrderAttack(std::weak_ptr<Unit> unit);
    void OrderInteraction(std::weak_ptr<IInteractableComponent> interactable);
    void ActivateSkill(SkillType::Enum skillType, Vector3d pos);
    void UnSelectSkill(bool forceUnselect = false);
    // 현재 선택된 스킬이 특정 유닛의 스킬이라면 선택을 취소시킨다.
    void UnSelectSkill(std::weak_ptr<Unit> unit);
    Vector3d GetWorldCursorPosition();
    // 플레이어 유닛의 위치들의 중점을 반환한다.
    Vector3d GetMiddlePoint();
    // 연속으로 쌓은 콤보를 초기화한다.
    void ResetCombo();
    void SetTacticCameraActive(bool boolen);

    void SetCooltime(SkillType::Enum skillType, float cooltime);
    void SetCooltime(std::weak_ptr<Unit> unit);
    float GetCooltimeForSkill(SkillType::Enum skillType);
    float RequiredManaForSkill(SkillType::Enum skillType);
    void ApplyHoverEffect(std::weak_ptr<Unit> unit);
    void DisableHoverEffect();
    void ApplySelectEffect(std::weak_ptr<Unit> unit);
    void ApplyTargetedEffect(std::weak_ptr<Unit> unit);
    void InitUnitMouseInteractionEffects();
    void EnableErrorUI(EnqueErrorType errorType);
    void OnPlayerUnitSkillActivation(std::weak_ptr<Unit> unit, std::weak_ptr<Skill> skill);
    void OnPlayerUnitSkillExpiration(std::weak_ptr<Unit> unit, std::weak_ptr<Skill> skill);
    std::vector<Vector3d>& ModifyPathForAttack(std::vector<Vector3d>& path);
    std::vector<Vector3d>& ModifyPathForSkill(std::vector<Vector3d>& path, SkillType::Enum skillType);
    void SetAttackMoveMode(bool attackMoveMode);
    std::unordered_set<PlaytimeWave*> finishedWavesCaptured;
    int skillButtonIDHovering;
    int parentEnumIDHovering;
    int currentCombo{ 0 };
    std::array<int, 3> comboObjective{ 10, 20, 30 };
    std::array<bool, 3> comboAchieved{ false };
    std::array<float, SkillType::SKILL_NUM> previousSkillCooltimeLeft;
    std::array<float, SkillType::SKILL_NUM> skillCooltimeLeft;
    std::array<UIElement*, SkillType::SKILL_NUM> skillCooltimeNumberUI;
    std::array<UIElement*, SkillType::SKILL_NUM> skillCooltimeMaskUI;
    Vector3d camPivotPoint;
    Vector3d camPreviousPivotPoint;
    Vector3d camContrainingDirection;
    bool isConstraingCamUpdateDirection{ false };
    bool blockSkillCancel{ false };
    bool playerSwitchable{ true };
    std::array<bool, SkillType::SKILL_NUM> blockSkillSelection{ false };
    float mana{ 0 };
    State::Enum state{ State::Battle };
    SkillType::Enum selectedSkill = SkillType::NONE;
    bool attackMoveMode{ false };
    PlayerCharacterType::Enum selectedCharacterType = PlayerCharacterType::None;
    std::weak_ptr<UnitAcquisitionBoxCollider> cursorUnitDetector;
    std::array<std::weak_ptr<Unit>, (int)PlayerCharacterType::Num> characters;
    std::array<UIElement*, (int)PlayerCharacterType::Num> charactersOutOfCamUI;
    std::weak_ptr<Unit> selectedCharacter;
    std::weak_ptr<Unit> selectedDebugCharacter;
    const application::editor::RegionData* camLockRegion{ nullptr };
    SkillUpgradeType::Enum skillUpgradeTarget;
    UIEnumID skillUpgradeUITarget;
    std::array<bool, SkillUpgradeType::END> skillUpgraded;
    std::array<bool, SkillUpgradeType::END> skillUpgradedCaptured;
    int skillPointsLeft{ 0 };
    int skillPointsLeftCaptured{ 0 };
    float camZoomFactor = 5;
    shared_ptr<float> zoomMultiplierByState;
    shared_ptr<float> zoomMultiplierByNonSelection;
    Vector3d camOffsetNorm;
    Quaternion camRotation;
    std::array<std::weak_ptr<Unit>, 2> peaceFollowingUnits;
    std::array<Vector3d, 2> peaceFollowingDestination;
    bool mouseInteractionEffectInitalized{ false };
    // 유닛이 선택되었을 떄의 효과
    GameObject* allySelectedEffect{ nullptr };
    yunutyEngine::graphics::StaticMeshRenderer* allySelectedEffectRenderer{ nullptr };

    std::shared_ptr<Reference::Guard> unitHoverOutlineGuard;
    std::shared_ptr<Reference::Guard> unitSelectOutlineGuard;
    // 유닛이 타겟으로 지정되었을 떄의 효과(사실상 평타 우클릭밖에 없음)
    std::array<GameObject*, 3> enemyTargetedEffect{ nullptr };
    std::array<VFXAnimator*, 3> enemyTargetedEffectAnimator{ nullptr };
    // 마우스 커서가 적 혹은 아군 위에 올라갈 때의 효과
    GameObject* allyHoverEffect{ nullptr };
    VFXAnimator* allyHoverEffectAnimator{ nullptr };
    yunutyEngine::graphics::StaticMeshRenderer* allyHoverEffectRenderer{ nullptr };

    GameObject* enemyHoverEffect{ nullptr };
    VFXAnimator* enemyHoverEffectAnimator{ nullptr };
    yunutyEngine::graphics::StaticMeshRenderer* enemyHoverEffectRenderer{ nullptr };

    GameObject* tacticCameraRef{ nullptr };

    State::Enum stateRequestedByAction = State::None;
    bool isStateAction = false;
    float elapsedTimeSinceLastCombo{ 0 };
    int localTimeScale{ 1 };
    friend application::Action_BlockSkillCancel;
    friend application::Action_BlockSkillSelection;
    friend application::Action_BlockPlayerSwitch;
};
