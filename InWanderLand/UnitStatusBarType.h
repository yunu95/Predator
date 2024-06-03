#pragma once
#include "PodStructs.h"

struct UnitStatusBarType
{
    enum Enum {
        NONE,
        PLAYER,
        ENEMY,
        ELITE,
        BOSS,
    };
};

template<>
const std::unordered_map<int, std::string>& application::POD_Enum<UnitStatusBarType::Enum>::GetEnumNameMap()
{
    static std::unordered_map<int, std::string> enumNameMap
    {
        {static_cast<int>(UnitStatusBarType::NONE), "NONE"},
        {static_cast<int>(UnitStatusBarType::PLAYER), "PLAYER"},
        {static_cast<int>(UnitStatusBarType::ENEMY), "ENEMY"},
        {static_cast<int>(UnitStatusBarType::ELITE), "ELITE"},
        {static_cast<int>(UnitStatusBarType::BOSS), "BOSS"},
    };
    return enumNameMap;
}
