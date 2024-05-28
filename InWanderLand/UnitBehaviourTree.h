#include "BehaviourTree.h"

class UnitBehaviourTree : public BehaviourTree
{
public:
    enum Keywords
    {
        Death,
        Paralysis,
        Knockback,
        Stun,
        Pause,
        Chasing,
        Attack,
        Move,
        AttackMove,
        Stop,
        Skill,
        Hold,
    };
};
