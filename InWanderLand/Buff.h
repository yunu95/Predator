#pragma once
#include <memory>
class Unit;

class UnitBuff
{
private:
    std::weak_ptr<Unit> owner;
public:
    UnitBuff(std::weak_ptr<Unit> owner);
    void Update();
};
