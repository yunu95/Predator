#pragma once
#include "ColorTintTimer.h"
#include "UIImage.h"
#include "InWanderLand.h"

void ColorTintTimer::Init()
{
    isRealtime = true;
    onUpdate = [=](float t)
        {
            t = UICurveFunctions[uiCurveType](t);
            uiImage->GetGI().SetColor({ math::LerpF(startColor.r,endColor.r,t),math::LerpF(startColor.g,endColor.g,t),math::LerpF(startColor.b,endColor.b,t),math::LerpF(startColor.a,endColor.a,t) });
        };
    onActivation = [=]()
        {
            uiImage->GetGI().SetColor(startColor);
        };
    if (disableOnEnd)
    {
        onCompleteFunction = [=]()
            {
                uiImage->GetGameObject()->SetSelfActive(false);
            };
    }
}