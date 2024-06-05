#pragma once
#include "YunutyEngine.h"
#include "SkillType.h"

class Unit;
class Skill
{
protected:
    Vector3d targetPos;
    std::weak_ptr<Unit> owner;
    std::weak_ptr<Skill> selfWeakPtr;
public:
    virtual coroutine::Coroutine operator()() = 0;
    virtual SkillType::Enum GetSkillType() { return SkillType::NONE; };
    virtual float GetCastRange() { return 8; };

    virtual void OnInterruption() {};

    friend Unit;
};
