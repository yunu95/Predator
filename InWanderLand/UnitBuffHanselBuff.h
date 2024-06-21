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
    std::weak_ptr<VFXAnimator> buffEffectAnimator;

    virtual void OnStart();
    virtual void OnUpdate();
    virtual void OnEnd();
    virtual void OnOverlap(UnitBuff&& overlapping) override;
    virtual void OnPause() override;
    virtual void OnResume() override;
};