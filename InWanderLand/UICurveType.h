#pragma once

enum class UICurveType
{
    None = 0,
    Linear,
    // 빠르게 진입 느리게 안착
    EaseOut,
    // 느리게 진입 빠르게 안착
    EaseIn,
    // 반대편으로 살짝 갔다가 빠르게 진입 느리게 안착
    Anticipation,
    // 빠르게 진입 느리게 안착
    EaseOutQuad,
    End,
};

extern float (*UICurveFunctions[(int)UICurveType::End])(float t);
