#pragma once
#include "PodStructs.h"

struct UnitStatusBarType
{
    enum Enum {
        NONE,
        PLAYER_ROBIN,
        PLAYER_URSULA,
        PLAYER_HANSEL,
        ENEMY,
        ELITE,
        BOSS,
        BOSS_FRAME
    };
};

template<>
const std::unordered_map<int, std::string>& application::POD_Enum<UnitStatusBarType::Enum>::GetEnumNameMap()
{
    static std::unordered_map<int, std::string> enumNameMap
    {
        {static_cast<int>(UnitStatusBarType::NONE), "NONE"},
        {static_cast<int>(UnitStatusBarType::PLAYER_ROBIN), "PLAYER_ROBIN"},
        {static_cast<int>(UnitStatusBarType::PLAYER_URSULA), "PLAYER_URSULA"},
        {static_cast<int>(UnitStatusBarType::PLAYER_HANSEL), "PLAYER_HANSEL"},
        {static_cast<int>(UnitStatusBarType::ENEMY), "ENEMY"},
        {static_cast<int>(UnitStatusBarType::ELITE), "ELITE"},
        {static_cast<int>(UnitStatusBarType::BOSS), "BOSS"},
        {static_cast<int>(UnitStatusBarType::BOSS_FRAME), "BOSS_FRAME"},
    };
    return enumNameMap;
}
