#pragma once

enum class DamageType
{
    NONE,
    // 평타 데미지
    Attack,
    // 치명타 평타 데미지
    AttackCrit,
    // 스킬 데미지
    Skill,
    // 기타 데미지
    Miscellaneous,
};