#pragma once
#include "UnitBuff.h"
#include "Reference.h"

class UnitBuffTaunted : public UnitBuff
{
public:
    virtual UnitBuffType GetBuffType() override;
    virtual UIEnumID GetUIEnumID();
    virtual void OnUpdate() override;
    std::shared_ptr<Reference::Guard> blockPendingCommand;
    std::weak_ptr<Unit> inflictor;
    UnitBuffTaunted(std::weak_ptr<Unit> inflictor);
private:
};