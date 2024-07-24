#pragma once
#include "PodStructs.h"

struct ShortcutType
{
    enum Enum
    {
        NONE,
        CTRL_ALT_1,
        CTRL_ALT_2,
        CTRL_ALT_3,
        CTRL_ALT_4,
        CTRL_ALT_5,
        CTRL_ALT_6,
        CTRL_ALT_7,
        CTRL_ALT_8,
        CTRL_ALT_9,
        CTRL_ALT_0,
    };
};

template<>
const std::unordered_map<int, std::string>& application::POD_Enum<ShortcutType::Enum>::GetEnumNameMap()
{
    // vim command
    // '<,'>g!/\/\//s/\w\+/{ static_cast<int>((int)ShortcutType::&), "&" }
    static std::unordered_map<int, std::string> enumNameMap
    {
        { static_cast<int>((int)ShortcutType::NONE), "NONE" },
        { static_cast<int>((int)ShortcutType::CTRL_ALT_1), "CTRL_ALT_1" },
        { static_cast<int>((int)ShortcutType::CTRL_ALT_2), "CTRL_ALT_2" },
        { static_cast<int>((int)ShortcutType::CTRL_ALT_3), "CTRL_ALT_3" },
        { static_cast<int>((int)ShortcutType::CTRL_ALT_4), "CTRL_ALT_4" },
        { static_cast<int>((int)ShortcutType::CTRL_ALT_5), "CTRL_ALT_5" },
        { static_cast<int>((int)ShortcutType::CTRL_ALT_6), "CTRL_ALT_6" },
        { static_cast<int>((int)ShortcutType::CTRL_ALT_7), "CTRL_ALT_7" },
        { static_cast<int>((int)ShortcutType::CTRL_ALT_8), "CTRL_ALT_8" },
        { static_cast<int>((int)ShortcutType::CTRL_ALT_9), "CTRL_ALT_9" },
        { static_cast<int>((int)ShortcutType::CTRL_ALT_0), "CTRL_ALT_0" },
    };
    return enumNameMap;
}