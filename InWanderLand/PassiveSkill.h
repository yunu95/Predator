#pragma once
#include "YunutyEngine.h"
#include "Skill.h"
#include "SkillType.h"

class Unit;
class PassiveSkill
{
public:
    // 패시브 스킬의 Init 함수는 유닛의 Summon 주기에서 호출됩니다.
    virtual void Init(std::weak_ptr<Unit> owner) = 0;
    virtual SkillType::Enum GetSkillType() { return SkillType::NONE; };
protected:
    std::weak_ptr<Unit> owner;
private:
};