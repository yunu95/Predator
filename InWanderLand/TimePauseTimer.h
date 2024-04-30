#pragma once
#include "YunutyEngine.h"
#include "Timer.h"

// 시간에 걸쳐 천천히 게임속도를 줄여 끝내 정지에 가깝게 하는 타이머 컴포넌트
class TimePauseTimer : public TimerComponent
{
public:
    static constexpr float pausedTimeScale = 0.0000001f;
    void Init();
};
