#pragma once
#include "YunutyEngine.h"
#include "UIEnumID.h"

class Unit;
class UIElement;
class UnitBuff
{
private:
    std::weak_ptr<UIElement> buffIcon1;
    std::weak_ptr<UIElement> buffIcon2;
    void Init(std::weak_ptr<Unit> owner);
protected:
    std::weak_ptr<Unit> owner;
public:
    float durationLeft = 1;
    enum class Type
    {
        RobinBleeding,
    };
    virtual Type GetSkillType() = 0;
    virtual UIEnumID GetUIEnumID() { return UIEnumID::None; };
    virtual void OnStart() {};
    virtual void OnUpdate() {};
    virtual void OnEnd() {};
    virtual ~UnitBuff();
    friend Unit;
};