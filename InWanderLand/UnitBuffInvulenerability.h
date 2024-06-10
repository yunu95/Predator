#pragma once
#include "Reference.h"
#include "UnitBuff.h"

class UnitBuffInvulenerability : public UnitBuff
{
public:
    virtual UnitBuffType GetBuffType() { return UnitBuffType::Invulnerability; }
    virtual UIEnumID GetUIEnumID() { return UIEnumID::None; };
    virtual void OnStart();
    UnitBuffInvulenerability(float duration)
    {
        durationLeft = duration;
    }
private:
    std::shared_ptr<Reference::Guard> invulnerabilityGuard;
};