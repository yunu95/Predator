#pragma once
#include "PodStructs.h"

struct PlayerCharacterType
{
    enum Enum {
        None = -1,
        Robin,
        Ursula,
        Hansel,
        Num,
    };
};

template<>
const std::unordered_map<int, std::string>& application::POD_Enum<PlayerCharacterType::Enum>::GetEnumNameMap()
{
    // vim command
    // '<,'>g!/\/\//s/\w\+/{ static_cast<int>((int)UnitDamageFontType::&), "&" }
    static std::unordered_map<int, std::string> enumNameMap
    {
        { (int)PlayerCharacterType::None, "None" },
        { (int)PlayerCharacterType::Robin, "Robin" },
        { (int)PlayerCharacterType::Ursula, "Ursula" },
        { (int)PlayerCharacterType::Hansel, "Hansel" },
        { (int)PlayerCharacterType::Num, "Num"}
    };
    return enumNameMap;
}
