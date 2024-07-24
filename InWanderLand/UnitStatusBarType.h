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
        BOSS_FRAME_LEFT,
        BOSS_FRAME_RIGHT,
    };
};

template<>
const std::unordered_map<int, std::string>& application::POD_Enum<UnitStatusBarType::Enum>::GetEnumNameMap()
{
    // vim command
    // '<,'>g!/\/\//s/\w\+/{ static_cast<int>((int)UnitDamageFontType::&), "&" }
    static std::unordered_map<int, std::string> enumNameMap
    {
        {static_cast<int>(UnitStatusBarType::NONE), "NONE"},
        {static_cast<int>(UnitStatusBarType::PLAYER_ROBIN), "PLAYER_ROBIN"},
        {static_cast<int>(UnitStatusBarType::PLAYER_URSULA), "PLAYER_URSULA"},
        {static_cast<int>(UnitStatusBarType::PLAYER_HANSEL), "PLAYER_HANSEL"},
        {static_cast<int>(UnitStatusBarType::ENEMY), "ENEMY"},
        {static_cast<int>(UnitStatusBarType::ELITE), "ELITE"},
        {static_cast<int>(UnitStatusBarType::BOSS), "BOSS"},
        {static_cast<int>(UnitStatusBarType::BOSS_FRAME_LEFT), "BOSS_FRAME_LEFT"},
        {static_cast<int>(UnitStatusBarType::BOSS_FRAME_RIGHT), "BOSS_FRAME_RIGHT"},
    };
    return enumNameMap;
}
