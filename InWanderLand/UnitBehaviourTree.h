#include "BehaviourTree.h"
#include "PodStructs.h"

class UnitBehaviourTree : public BehaviourTree
{
public:
    enum Keywords
    {
        Death = 0,
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
        KeywordNum,
    };
};
template<>
const std::unordered_map<int, std::string>& application::POD_Enum<UnitBehaviourTree::Keywords>::GetEnumNameMap()
{
    static std::unordered_map<int, std::string> enumNameMap
    {
        {UnitBehaviourTree::Death, "Death"},
        {UnitBehaviourTree::Paralysis, "Paralysis"},
        {UnitBehaviourTree::Knockback, "Knockback"},
        {UnitBehaviourTree::Stun, "Stun"},
        {UnitBehaviourTree::Pause, "Pause"},
        {UnitBehaviourTree::Chasing, "Chasing"},
        {UnitBehaviourTree::Attack, "Attack"},
        {UnitBehaviourTree::Move, "Move"},
        {UnitBehaviourTree::AttackMove, "AttackMove"},
        {UnitBehaviourTree::Stop, "Stop"},
        {UnitBehaviourTree::Skill, "Skill"},
        {UnitBehaviourTree::Hold, "Hold"},
    };
    return enumNameMap;
}
