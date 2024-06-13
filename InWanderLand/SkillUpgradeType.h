#pragma once

struct SkillUpgradeType
{
    enum Enum
    {
        NONE,
        //지속 피해 효과 적용 시간이 증가합니다.
        //돌진 사거리가 증가합니다.
        //고정 피해량이 증가합니다.
        //고정 피해량이 증가합니다.
        //스킬 범위가 증가합니다.
        ROBIN_PASSIVE_LONGER,
        ROBIN_Q_RANGE,
        ROBIN_Q_DAMAGE,
        ROBIN_W_DAMAGE,
        ROBIN_W_RADIUS,
        //공격 강화 버프가 강화됩니다.
        //고정 피해량이 증가합니다.
        //스킬 사거리가 증가합니다.
        //고정 피해량이 증가합니다.
        //스킬 범위가 증가합니다.
        URSULA_PASSIVE_ENHANCE,
        URSULA_Q_DAMAGE,
        URSULA_Q_RANGE,
        URSULA_W_DAMAGE,
        URSULA_W_RADIUS,
        //체력 회복 아이템의 회복량이 증가합니다.
        //고정 피해량이 증가합니다.
        //스킬 사거리가 증가합니다.
        //스킬 사거리가 증가합니다.
        //스킬 타격 횟수가 증가합니다.
        HANSEL_PASSIVE_ENHANCE,
        HANSEL_Q_DAMAGE,
        HANSEL_Q_RANGE,
        HANSEL_W_RANGE,
        HANSEL_W_MORE_HITS,
        END
    };
};
