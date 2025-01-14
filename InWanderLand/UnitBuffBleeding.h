#pragma once
#include "UnitBuff.h"

class UnitBuffBleeding : public UnitBuff
{
private:
    // 피해를 가하는 유닛
    std::weak_ptr<Unit> Inflictor;
    float damagePerSecond{ 0 };
    float accumulatedDeltaTime{ 0 };
    static constexpr float dmgInterval = 1.0f;
    bool isPaused = false;

public:
    virtual void OnStart();
    virtual void OnUpdate();
    virtual void OnEnd();

    virtual void OnPause() override;
    virtual void OnResume() override;

    virtual UnitBuffType GetBuffType() { return UnitBuffType::RobinBleeding; }
    virtual UIEnumID GetUIEnumID() { return UIEnumID::CharInfo_Buff_Bleeding; };
    UnitBuffBleeding(std::weak_ptr<Unit> Inflictor, float damagePerSecond, float duration)
        :Inflictor(Inflictor), damagePerSecond(damagePerSecond)
    {
        durationLeft = duration;
    }
    virtual void OnOverlap(UnitBuff&& overlapping) override;
};
