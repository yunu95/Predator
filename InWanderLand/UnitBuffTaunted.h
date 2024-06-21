#pragma once
#include "UnitBuff.h"
#include "Reference.h"

class UnitBuffTaunted : public UnitBuff
{
public:
    virtual UnitBuffType GetBuffType() override;
    virtual UIEnumID GetUIEnumID();
    virtual void OnStart() override;
    virtual void OnEnd() override;
    virtual void OnUpdate() override;
    virtual void OnPause() override;
    virtual void OnResume() override;
    virtual void OnOverlap(UnitBuff&& overlapping) override;
    std::shared_ptr<Reference::Guard> blockPendingCommand;
    std::weak_ptr<Unit> inflictor;
    UnitBuffTaunted(std::weak_ptr<Unit> inflictor);
private:
    std::weak_ptr<VFXAnimator> buffEffectAnimator;
};