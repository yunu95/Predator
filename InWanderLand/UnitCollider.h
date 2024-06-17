#pragma once
#include "YunutyEngine.h"

class Unit;
class UnitCollider : virtual public Component
{
public:
    std::weak_ptr<Unit> owner;
    static Unit* AcquireUnit(physics::Collider* collider);
private:
};