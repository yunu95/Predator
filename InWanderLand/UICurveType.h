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
};

template<UICurveType curveType>
float GetUICurveVal(float t);

template<> float GetUICurveVal<UICurveType::Linear>(float t) { return t; }
template<> float GetUICurveVal<UICurveType::EaseOut>(float t) { return 1 - (1 - t) * (1 - t); }
template<> float GetUICurveVal<UICurveType::EaseIn>(float t) { return t * t; }
template<> float GetUICurveVal<UICurveType::Anticipation>(float t)
{
    // cubic
    float t1 = t;
    float t2 = t * t;
    float t3 = t * t * t;
    if (t < 0.36009951)
    {
        return -1.361598 * t3 + 5.322844 * t2 - 1.883897 * t1 - 0.05047305;
    }
    else
    {
        return -6.077918 * t3 + 5.195014 * t2 + 0.806061 * t1 + 1;
    }
}
