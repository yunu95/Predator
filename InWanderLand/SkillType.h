#pragma once
#include "PodStructs.h"

struct SkillType
{
    enum Enum
    {
        NONE = -1,
        ROBIN_Q,
        ROBIN_W,
        URSULA_Q,
        URSULA_W,
        HANSEL_Q,
        HANSEL_W,
        EnemySpin,
        EnemyImpale,
        BossSkill_One,
        BossSkill_Two,
        BossSkill_Three,
        BossSkill_Four,
        Passive_Robin_Bleeding,
        Passive_Ursula_Speed,
        Passive_Hansel_Heal,
        SKILL_NUM
    };
};

template<>
const std::unordered_map<int, std::string>& application::POD_Enum<SkillType::Enum>::GetEnumNameMap()
{
    static std::unordered_map<int, std::string> enumNameMap
    {
        { (int)SkillType::NONE, "None" },
        { (int)SkillType::ROBIN_Q, "Robin_Q" },
        { (int)SkillType::ROBIN_W, "Robin_W" },
        { (int)SkillType::URSULA_Q, "Ursula_Q" },
        { (int)SkillType::URSULA_W, "Ursula_W" },
        { (int)SkillType::HANSEL_Q, "Hansel_Q" },
        { (int)SkillType::HANSEL_W, "Hansel_W" },
        { (int)SkillType::EnemySpin, "EnemySpin" },
        { (int)SkillType::EnemyImpale, "EnemyImpale" },
        { (int)SkillType::BossSkill_One, "BossSkill_One" },
        { (int)SkillType::BossSkill_Two, "BossSkill_Two" },
        { (int)SkillType::BossSkill_Three, "BossSkill_Three" },
        { (int)SkillType::BossSkill_Four, "BossSkill_Four" },
        { (int)SkillType::Passive_Robin_Bleeding, "Passive_Robin_Bleeding" },
        { (int)SkillType::SKILL_NUM, "Skill_Num"}
    };
    return enumNameMap;
}
