#pragma once
#include "UnitBuff.h"
#include "Reference.h"

class UnitBuffBlinded : public UnitBuff
{
public:
    virtual UnitBuffType GetBuffType() override;
    virtual UIEnumID GetUIEnumID();
    virtual void OnStart() override;
    virtual void OnEnd() override;
    virtual void OnOverlap(UnitBuff&& overlapping) override;

    virtual void OnPause() override;
    virtual void OnResume() override;

    std::shared_ptr<Reference::Guard> blindReference;
private:
    virtual coroutine::Coroutine EffectCoroutine() override;
    std::weak_ptr<VFXAnimator> buffEffectAnimator;
};