#pragma once
#include "UnitBuff.h"

class UnitBuffBleeding : public UnitBuff
{
private:
    // 피해를 가하는 유닛
    std::weak_ptr<Unit> Inflictor;
public:
    virtual void OnUpdate();
    virtual Type GetSkillType() { return Type::RobinBleeding; }
    virtual UIEnumID GetUIEnumID() { return UIEnumID::CharInfo_Buff_Bleeding; };
    UnitBuffBleeding(std::weak_ptr<Unit> Inflictor)
    {
        UnitBuff::durationLeft = 1;
    }
};