#pragma once
#include "PodStructs.h"

struct ProjectileType
{
    enum Enum 
    {
        CURVE,
        DIRECT,
    };
};

template<>
const std::unordered_map<int, std::string>& application::POD_Enum<ProjectileType::Enum>::GetEnumNameMap()
{
    // vim command
    // '<,'>g!/\/\//s/\w\+/{ static_cast<int>((int)UnitDamageFontType::&), "&" }
    static std::unordered_map<int, std::string> enumNameMap
    {
        {static_cast<int>(ProjectileType::CURVE), "CURVE"},
        {static_cast<int>(ProjectileType::DIRECT), "DIRECT"},
    };
    return enumNameMap;
}
