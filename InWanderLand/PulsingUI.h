#pragma once
#include "Timer.h"
//#include "curve.h"

// 사이즈가 max와 min 사이를 일정간격으로 오가는 컴포넌트
class PulsingUI : public TimerComponent
{
public:
    float pulsingMin{ 0 };
    float pulsingMax{ 1 };
    virtual void Start() override;
    void Init();
};
