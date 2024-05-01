#pragma once
#include "YunutyEngine.h"
#include "UICurveType.h"
#include "Timer.h"

struct JsonUIData;
class UIElement;
// 게임오브젝트가 활성화될때 스케일을 0에서 확 늘리는 컴포넌트
class ColorTintTimer : public TimerComponent
{
public:
    graphics::UIImage* uiImage;
    yunuGI::Color startColor;
    yunuGI::Color endColor;
    int uiCurveType;
    bool disableOnEnd;
    void Init();
};
