#pragma once
#include "YunutyEngine.h"

class Unit;
class PassiveSkill
{
public:
    // 패시브 스킬의 Init 스킬은 유닛의 Summon 주기에서 호출됩니다.
    virtual void Init(std::weak_ptr<Unit> owner);
protected:
private:
    std::weak_ptr<Unit> owner;
};