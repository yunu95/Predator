#pragma once
#include "UnitBuff.h"

// 아군은 공격속도 증진, 적군은 공격속도 감소
class UnitBuffHanselBuff : public UnitBuff
{
public:
    UnitBuffHanselBuff();
    std::shared_ptr<float> attackSpeedAdder;
    std::shared_ptr<float> critChanceAdder;
    virtual UIEnumID GetUIEnumID() { return UIEnumID::CharInfo_Buff_HanselBuff; };
    virtual UnitBuffType GetBuffType() { return UnitBuffType::HanselBuff; };
private:
    virtual void OnStart();
};