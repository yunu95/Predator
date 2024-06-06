#pragma once
#include "PodStructs.h"

struct UnitControllerType
{
    enum Enum
    {
        NONE = -1,
        // 성난 군중, 그저 어택땅만하는 놈들
        ANGRY_MOB,
        // 근접 정예병
        MELEE_ELITE,
        // 원거리 정예병
        RANGED_ELITE,
        // 하트퀸
        HEART_QUEEN,
        NUM
    };
};

template<>
const std::unordered_map<int, std::string>& application::POD_Enum<UnitControllerType::Enum>::GetEnumNameMap()
{
    static std::unordered_map<int, std::string> enumNameMap
    {
        { (int)UnitControllerType::NONE, "None" },
        {(int)UnitControllerType::ANGRY_MOB, "Angry Mob"},
        {(int)UnitControllerType::MELEE_ELITE, "Elite Melee"},
        {(int)UnitControllerType::RANGED_ELITE, "Elite Ranged"},
        {(int)UnitControllerType::HEART_QUEEN, "Heart Queen"},
        {(int)UnitControllerType::NUM, "Controller Type Num"}
    };
    return enumNameMap;
}
