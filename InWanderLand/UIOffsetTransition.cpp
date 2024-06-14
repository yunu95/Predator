#include "JsonUIData.h"
#include "UIOffsetTransition.h"

void UIOffsetTransition::Init(const JsonUIData& jsonData, bool isEnableEffect)
{
    Vector3d startPosition = GetTransform()->GetLocalPosition();
    Vector3d endPosition = GetTransform()->GetLocalPosition();
    float delay = 0;
    int curveType;

    if (isEnableEffect)
    {
        startPosition += Vector3d{ jsonData.enableOffset[0],-jsonData.enableOffset[1], 0 };
        delay = jsonData.floats[JsonUIFloatType::enableDelay];
        pushDuration = delay + jsonData.floats[JsonUIFloatType::enableDuration];
        curveType = jsonData.enableCurveType;
    }
    else
    {
        endPosition += Vector3d{ jsonData.disableOffset[0],-jsonData.disableOffset[1], 0 };
        delay = jsonData.floats[JsonUIFloatType::disableDelay];
        pushDuration = delay + jsonData.floats[JsonUIFloatType::disableDuration];
        curveType = jsonData.disableCurveType;
    }
    isRealtime = true;
    onCompleteFunction = [=]()
        {
            GetTransform()->SetLocalPosition(endPosition);
            GetGameObject()->SetSelfActive(isEnableEffect);
        };
    onUpdate = [=](float t)
        {
            t = max(0.0f, (t * pushDuration - delay) / (pushDuration - delay));
            GetTransform()->SetLocalPosition(Vector3d::Lerp(startPosition, endPosition, UICurveFunctions[curveType](t)));
        };
    onActivation = [=]()
        {
            GetTransform()->SetLocalPosition(startPosition);
            GetGameObject()->SetSelfActive(true);
        };
};