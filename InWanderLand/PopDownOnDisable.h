#pragma once
#include "YunutyEngine.h"
#include "Timer.h"

// 게임오브젝트가 활성화될때 스케일을 0에서 확 늘리는 컴포넌트
class PopDownOnDisable : public TimerComponent
{
public:
    Vector3f popDownFrom;
    float popDownTo;
    bool x = true, y = false, z = false;
    void Init();
};
