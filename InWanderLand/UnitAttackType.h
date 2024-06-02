#pragma once
#include "PodStructs.h"

struct UnitAttackType
{
    enum Enum {
        NONE,
        MELEE,
        MISSILE,
    };
};

template<>
const std::unordered_map<int, std::string>& application::POD_Enum<UnitAttackType::Enum>::GetEnumNameMap()
{
    static std::unordered_map<int, std::string> enumNameMap
    {
        {static_cast<int>(UnitAttackType::NONE), "NONE"},
        {static_cast<int>(UnitAttackType::MELEE), "MELEE"},
        {static_cast<int>(UnitAttackType::MISSILE), "MISSILE"},
    };
    return enumNameMap;
}

