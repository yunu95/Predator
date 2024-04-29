#pragma once
#include "YunutyEngine.h"
#include "Timer.h"
//#include "curve.h"

// 게임오브젝트가 활성화될때 스케일을 0에서 확 늘리는 컴포넌트
class PopupOnEnable : public TimerComponent
{
public:
    bool x = true, y = false, z = false;
    void Init();
};
