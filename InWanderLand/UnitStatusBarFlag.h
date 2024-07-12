#pragma once

struct UnitStatusBarFlag
{
    enum Enum {
        NONE = 0,
        FollowUnit = 1 << 0,
        DestroyOnDeath = 1 << 1,
        ControlWithReallyDisabled = 1 << 2,
        EnableAfterDamaged = 1 << 3,
    };
};