#pragma once
#include "YunutyEngine.h"
#include "SkillType.h"

class Unit;
class Skill
{
private:
    std::weak_ptr<Unit> owner;
public:
    virtual coroutine::Coroutine operator()() = 0;
    virtual SkillType GetSkillType() { return SkillType::NONE; };
    friend Unit;
};
