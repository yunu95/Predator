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
            if (disableOnEnd)
                startColor = uiImage->GetGI().GetColor();
            uiImage->GetGI().SetColor(startColor);
        };
    if (disableOnEnd)
    {
        onCompleteFunction = [=]()
            {
                uiImage->GetGI().SetColor(startColor);
                uiImage->GetGameObject()->SetSelfActive(false);
            };
    }
}