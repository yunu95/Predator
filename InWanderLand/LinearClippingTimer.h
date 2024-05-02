#pragma once
#include "YunutyEngine.h"
#include "UICurveType.h"
#include "Timer.h"

struct JsonUIData;
class UIElement;
// 게임오브젝트가 활성화될때 스케일을 0에서 확 늘리는 컴포넌트
class LinearClippingTimer : public TimerComponent
{
public:
    graphics::UIImage* uiImage;
    yunuGI::Vector2 startPos;
    yunuGI::Vector2 clipDirection;
    int uiCurveType;
    bool disableOnEnd;
    bool reverseOffset = false;
    void Init();
};
