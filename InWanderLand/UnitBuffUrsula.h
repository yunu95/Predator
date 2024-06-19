#pragma once
#include "UnitBuff.h"
#include "InWanderLand.h"

// 스스로의 공격속도, 공격력 증가
class UnitBuffUrsula : public UnitBuff
{
public:
    UnitBuffUrsula();
    std::shared_ptr<float> attackSpeedAdder;
    std::shared_ptr<float> critChanceAdder;
    virtual UnitBuffType GetBuffType() { return UnitBuffType::UrsulaBuff; };
    virtual UIEnumID GetUIEnumID() { return UIEnumID::CharInfo_Buff_UrsulaSelf; };
private:
    virtual void OnStart();
    virtual void OnUpdate() {};
    virtual void OnEnd();
    virtual void OnOverlap(UnitBuff&& overlapping);
    virtual coroutine::Coroutine EffectCoroutine() override;
};
