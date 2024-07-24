#pragma once
#include "PodStructs.h"

struct EffectSoundType
{
    enum Enum {
        None = -1,
        Projectile_Ursula,
        Projectile_Hansel,
        Projectile_RangedEnemy,
        HitSkill_Robin_Q_RushHit,
        HitSkill_Robin_W_Taunt,
        HitSkill_Ursula_Q,
        HitSkill_Ursula_W,
        HitSkill_Hansel_Q,
        HitSkill_Hansel_W_PieDisappear,
        HitSkill_Hansel_W_Buff_Friends,
        HitSkill_Hansel_W_Buff_Enemies,
        Interaction_Hansel_Passive_Cake,
        Num,
    };
};

template<>
const std::unordered_map<int, std::string>& application::POD_Enum<EffectSoundType::Enum>::GetEnumNameMap()
{
    // vim command
    // '<,'>g!/\/\//s/\w\+/{ static_cast<int>((int)UnitDamageFontType::&), "&" }
    static std::unordered_map<int, std::string> enumNameMap
    {
        { (int)EffectSoundType::None, "None" },
        { (int)EffectSoundType::Projectile_Ursula, "Projectile_Ursula" },
        { (int)EffectSoundType::Projectile_Hansel, "Projectile_Hansel" },
        { (int)EffectSoundType::Projectile_RangedEnemy, "Projectile_RangedEnemy" },
        { (int)EffectSoundType::HitSkill_Robin_Q_RushHit, "HitSkill_Robin_Q_RushHit" },
        { (int)EffectSoundType::HitSkill_Robin_W_Taunt, "HitSkill_Robin_W_Taunt" },
        { (int)EffectSoundType::HitSkill_Ursula_Q, "HitSkill_Ursula_Q" },
        { (int)EffectSoundType::HitSkill_Ursula_W, "HitSkill_Ursula_W" },
        { (int)EffectSoundType::HitSkill_Hansel_Q, "HitSkill_Hansel_Q" },
        { (int)EffectSoundType::HitSkill_Hansel_W_PieDisappear, "HitSkill_Hansel_W_PieDisappear" },
        { (int)EffectSoundType::HitSkill_Hansel_W_Buff_Friends, "HitSkill_Hansel_W_Buff_Friends" },
        { (int)EffectSoundType::HitSkill_Hansel_W_Buff_Enemies, "HitSkill_Hansel_W_Buff_Enemies" },
        { (int)EffectSoundType::Interaction_Hansel_Passive_Cake, "Interaction_Hansel_Passive_Cake" },
        { (int)EffectSoundType::Num, "Num"}
    };
    return enumNameMap;
}
