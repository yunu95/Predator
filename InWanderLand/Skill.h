#pragma once
#include "YunutyEngine.h"

class Unit;
class Skill
{
private:
    std::weak_ptr<Unit> owner;
public:
    virtual coroutine::Coroutine operator()() = 0;
    friend Unit;
};
