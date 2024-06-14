#pragma once

enum class UnitAnimType
{
    None,
    Idle,
    BattleIdle,
    Move,
    BattleMove,
    Attack,
    Death,
    Paralysis, // 경직 동작
    Slam,   // 땅을 쿵 내려찍는 동작
    Taunt,   // 도발 동작
    Skill1,  // 스킬1
    Skill2,  // 스킬2
    Skill3,  // 스킬3
    Skill4,  // 스킬4
    Birth,  // 탄생 혹은 부활
    Rush,   // 돌진
    Throw,
    Spin,
    BattleStart,  // 전투 시작
    BattleEnd,   // 전투 끝
    Damaged,
    End
};

namespace Animation
{
    typedef int PlayFlag;
    enum PlayFlag_
    {
        None = 0,
        Blending = 1 << 0,
        Repeat = 1 << 1,
    };
}
