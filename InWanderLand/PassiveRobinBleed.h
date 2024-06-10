#pragma once
#include "PassiveSkill.h"

class PassiveRobinBleed : public PassiveSkill
{
public:
    virtual void Init(std::weak_ptr<Unit> owner);
private:
};