#pragma once
#include "YunutyEngine.h"

// Tell, Don't ask 원칙을 정면으로 거스르는 클래스
// 유저의 전반적인 상태를 관장한다.
class PlayerStatusManager : public SingletonComponent<PlayerStatusManager>, public Component
{
private:
};
