#pragma once
#include "YunutyEngine.h"
#include <map>
#include "PermanentObservee.h"
#include "UnitController.h"
#include "UIManager.h"

// 사용자 입력을 받고 플레이어 유닛들의 행동을 관리하는 클래스
// 짬통 중의 짬통이다.
class RTSCam;
class PlayerUnit;
class CursorDetector;
class IInteractableComponent;
class UnitAcquisitionSphereCollider;

class PlayerController : public SingletonComponent<PlayerController>, public UnitController, public PermanentObservee
{
public:
    static constexpr int playerTeamIndex = 1;
    enum CharacterType { NONE = -1, ROBIN, URSULA, HANSEL, CHARACTER_NUM };
    enum class SkillType { NONE = -1, ROBIN_Q, ROBIN_W, URSULA_Q, URSULA_W, HANSEL_Q, HANSEL_W, SKILL_NUM };
    // 한번 호출한 후 내용을 다 지워버리는 휘발성 콜백들
    VolatileCallbacks onSkillSelect[(int)SkillType::SKILL_NUM];
    VolatileCallbacks onSkillActivate[(int)SkillType::SKILL_NUM];
private:
    SkillType selectedSkill = SkillType::NONE;
    CharacterType selectedCharacter = CharacterType::NONE;
    virtual Component* GetComponent() override { return this; }
    virtual void Start() override;
    virtual void OnContentsPlay() override;
    virtual void OnContentsStop() override;
    virtual void Update() override;
    // 사용자 입력을 받기 위해 매 프레임 불린다.
    void HandleInput();
    void SelectPlayerUnit(CharacterType charType);
    // character가 NONE일 경우 알아서 현재 선택된 스킬로 귀결된다.
    void OnLeftClick();
    void OnRightClick();
    void SelectUnit(std::weak_ptr<Unit> unit);
    void OrderMove(Vector3d position);
    void OrderAttack(std::weak_ptr<Unit> unit);
    void OrderInteraction(std::weak_ptr<IInteractableComponent> unit);
    void ActivateSkill(SkillType skillType);
    void SelectSkill(SkillType skillType);
    void UnSelectSkill(SkillType skillType);
    Vector3d GetWorldCursorPosition();
    std::weak_ptr<UnitAcquisitionSphereCollider> cursorUnitDetector;
    std::weak_ptr<Unit> characters[CHARACTER_NUM];
    std::weak_ptr<Unit> selectedCharacter;
};
