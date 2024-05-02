#pragma once
#include "LinearClippingTimer.h"
#include "UIImage.h"
#include "InWanderLand.h"

void LinearClippingTimer::Init()
{
    isRealtime = true;
    onUpdate = [=](float t)
        {
            t = UICurveFunctions[uiCurveType](t);
            uiImage->GetGI().SetLinearClipping(true);
            uiImage->GetGI().SetLinearClippingDirection(clipDirection.x, clipDirection.y);
            if (reverseOffset)
            {
                uiImage->GetGI().SetLinearClippingStartPoint(startPos.x - clipDirection.x * t, startPos.y - clipDirection.y * t);
            }
            else
            {
                uiImage->GetGI().SetLinearClippingStartPoint(startPos.x + clipDirection.x * t, startPos.y + clipDirection.y * t);
            }
        };
    if (disableOnEnd)
    {
        onCompleteFunction = [=]()
            {
                uiImage->GetGameObject()->SetSelfActive(false);
            };
    }
}