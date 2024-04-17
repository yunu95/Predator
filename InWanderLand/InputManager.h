#pragma once
#include "YunutyEngine.h"

class RTSCam;

/// <summary>
/// 키와 마우스의 입력을 받으면 해당 입력에 따라 다른 클래스의 함수를 호출해주는 함수.
///	입력과 전달의 역할만 갖는다.
/// </summary>
class InputManager : public Component, public SingletonComponent<InputManager>
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
    bool tacticMode = false;
    bool isMouseOnUIButton = false;

public:
    virtual void Start() override;
    virtual void Update() override;

    void SelectPlayer(Unit::UnitType p_unitType);
    void PrepareSkill(Unit::SkillEnum p_skillType, Unit::UnitType p_unitType);
    void PrepareSkill(Unit::SkillEnum p_skillType);
    void ToggleTacticMode();

    RTSCam* rtscam;
};

