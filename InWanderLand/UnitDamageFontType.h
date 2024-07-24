#pragma once
#include "PodStructs.h"

struct UnitDamageFontType
{
    enum Enum {
        None = -1,
        Red,
        Blue,
        BlackAndWhite,
        Cyan,
    };
};

template<>
const std::unordered_map<int, std::string>& application::POD_Enum<UnitDamageFontType::Enum>::GetEnumNameMap()
{
    // vim command
    // '<,'>g!/\/\//s/\w\+/{ static_cast<int>((int)UnitDamageFontType::&), "&" }
    static std::unordered_map<int, std::string> enumNameMap
    {
        { static_cast<int>((int)UnitDamageFontType::None), "None" },
        { static_cast<int>((int)UnitDamageFontType::Red), "Red" },
        { static_cast<int>((int)UnitDamageFontType::Blue), "Blue" },
        { static_cast<int>((int)UnitDamageFontType::BlackAndWhite), "BlackAndWhite" },
        { static_cast<int>((int)UnitDamageFontType::Cyan), "Cyan" },
    };
    return enumNameMap;
}
