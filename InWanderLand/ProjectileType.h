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
    static std::unordered_map<int, std::string> enumNameMap
    {
        {static_cast<int>(ProjectileType::CURVE), "CURVE"},
        {static_cast<int>(ProjectileType::DIRECT), "DIRECT"},
    };
    return enumNameMap;
}

struct ProjectileHoming
{
    enum Enum
    {
        YES,
        NO,
    };
};

template<>
const std::unordered_map<int, std::string>& application::POD_Enum<ProjectileHoming::Enum>::GetEnumNameMap()
{
    static std::unordered_map<int, std::string> enumNameMap
    {
        {static_cast<int>(ProjectileHoming::YES), "YES"},
        {static_cast<int>(ProjectileHoming::NO), "NO"},
    };
    return enumNameMap;
}

