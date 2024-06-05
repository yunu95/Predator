#pragma once
#include "YunutyEngine.h"
#include "SkillType.h"

class Unit;
class Skill
{
protected:
    float castRange = 10;
    Vector3d targetPos;
    std::weak_ptr<Unit> owner;
    std::weak_ptr<Skill> selfWeakPtr;
public:
    virtual coroutine::Coroutine operator()() = 0;
    virtual SkillType::Enum GetSkillType() { return SkillType::NONE; };

    virtual void OnInterruption() {};

    friend Unit;
};
