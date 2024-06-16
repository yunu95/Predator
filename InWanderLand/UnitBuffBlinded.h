#pragma once
#include "UnitBuff.h"
#include "Reference.h"

class UnitBuffBlinded : public UnitBuff
{
public:
    virtual UnitBuffType GetBuffType() override;
    virtual UIEnumID GetUIEnumID();
    virtual void OnStart() override;
    std::shared_ptr<Reference::Guard> blindReference;
private:
};