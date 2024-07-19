#include "UICurveType.h"

template<UICurveType curveType>
float GetUICurveVal(float t);

template<> float GetUICurveVal<UICurveType::Linear>(float t) { return t; }
template<> float GetUICurveVal<UICurveType::EaseOut>(float t) { return 1 - (1 - t) * (1 - t); }
template<> float GetUICurveVal<UICurveType::EaseIn>(float t) { return t * t; }
template<> float GetUICurveVal<UICurveType::Anticipation>(float t)
{
    // cubic
    if (t < 0.3609951)
    {
        t = t / 0.3609951;
        float t1 = t;
        float t2 = t * t;
        float t3 = t * t * t;
        return -0.06405493 * t3 + 0.6936593 * t2 - 0.6800774 * t1;
    }
    else
    {
        t = (t - 0.3609951) / 0.6390049;
        float t1 = t;
        float t2 = t * t;
        float t3 = t * t * t;
        return -1.58587 * t3 + 2.121266 * t2 + 0.5150764 * t1 + -0.05047305;
    }
}
template<> float GetUICurveVal<UICurveType::EaseOutQuad>(float t) { return 1 - (1 - t) * (1 - t) * (1 - t) * (1 - t); }

float (*UICurveFunctions[(int)UICurveType::End])(float t)
{
    GetUICurveVal<UICurveType::EaseOut>,
        GetUICurveVal<UICurveType::Linear>,
        GetUICurveVal<UICurveType::EaseOut>,
        GetUICurveVal<UICurveType::EaseIn>,
        GetUICurveVal<UICurveType::Anticipation>,
        GetUICurveVal<UICurveType::EaseOutQuad>,
};
