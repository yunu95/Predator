#pragma once
#include "PodStructs.h"

// 유닛은 여러 개의 유닛 컨트롤러 클래스에 소속되어 있을 수 있으나, 단 하나의 UnitControllerType만 가질 수 있습니다.
// 그러므로 유닛 컨트롤러 타입의 이름은 반드시 유닛 컨트롤러 클래스의 이름과 일대일 대응되지는 않습니다.
struct UnitControllerType
{
    enum Enum
    {
        NONE = -1,
        // 플레이어 유닛
        PLAYER,
        // 성난 군중, 그저 어택땅만하는 놈들
        ANGRY_MOB,
        // 근접 정예병
        MELEE_ELITE,
        // 원거리 정예병
        RANGED_ELITE,
        // 하트퀸
        HEART_QUEEN,
        // 제자리만 고수하는 유닛들
        HOLDER,
        // 거리를 유지하며 공격하려는 원거리 공격수
        RANGED_KITING,
        // 거리를 좁히며 다가오는 원거리 공격수
        RANGED_APPROACHING,
        NUM
    };
};

template<>
const std::unordered_map<int, std::string>& application::POD_Enum<UnitControllerType::Enum>::GetEnumNameMap()
{
    // vim command
    // '<,'>g!/\/\//s/\w\+/{ static_cast<int>((int)UnitDamageFontType::&), "&" }
    static std::unordered_map<int, std::string> enumNameMap
    {
        { (int)UnitControllerType::NONE, "None" },
        { (int)UnitControllerType::PLAYER, "Player"},
        {(int)UnitControllerType::ANGRY_MOB, "Angry Mob"},
        {(int)UnitControllerType::MELEE_ELITE, "Elite Melee"},
        {(int)UnitControllerType::RANGED_ELITE, "Elite Ranged"},
        {(int)UnitControllerType::HEART_QUEEN, "Heart Queen"},
        {(int)UnitControllerType::HOLDER, "Holder"},
        {(int)UnitControllerType::RANGED_KITING, "Ranged Kiting"},
        {(int)UnitControllerType::RANGED_APPROACHING, "Ranged Approaching"},
        {(int)UnitControllerType::NUM, "Controller Type Num"},
    };
    return enumNameMap;
}
