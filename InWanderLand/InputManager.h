#pragma once
#include "YunutyEngine.h"
#include "PermanentObservee.h"
#include "Unit.h"

class RTSCam;

/// <summary>
/// 키와 마우스의 입력을 받으면 해당 입력에 따라 다른 클래스의 함수를 호출해주는 함수.
///	입력과 전달의 역할만 갖는다.
/// </summary>
class InputManager : public SingletonComponent<InputManager>, public PermanentObservee
{
public:
    enum SelectedSerialNumber
    {
        One = 1,
        Two,
        Three,
        All
    };

private:
    SelectedSerialNumber currentSelectedSerialNumber;
    bool isPlayerSelected = false;
    //bool tacticMode = false;
    bool isMouseOnUIButton = false;
    bool isInputManagerActivated{ true };
public:
    InputManager();
    virtual void Start() override;
    virtual void Update() override;

    virtual void OnContentsStop() override;

    bool GetInputManagerActive();
    void SetInputManagerActive(bool p_boolen);
    void SelectPlayer(Unit::UnitType p_unitType);
    void PrepareSkill(Unit::SkillEnum p_skillType, Unit::UnitType p_unitType);
    void PrepareSkill(Unit::SkillEnum p_skillType);
    void ToggleTacticMode();

    // Skill을 Prepare할수 있는지의 여부
    unordered_map<Unit::UnitType, unordered_map<Unit::SkillEnum, bool>> canPrepareSkill;
    RTSCam* rtscam;
};

