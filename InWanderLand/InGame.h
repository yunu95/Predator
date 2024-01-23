#pragma once
#include "YunutyEngine.h"

// 인게임 플레이에 영향을 주는 전역 함수들을 모아놓은 정적 클래스입니다.
class InGame
{
public:
    // 현재 게임이 진행중인지 여부를 반환합니다.
    // 전술모드는 게임이 진행중인 상태입니다.
    // UI창이 열려있는 경우는 게임이 진행중이지 않은 상태입니다.
    // 에디터 기능을 실행할 때는 게임이 진행중이지 않은 상태이여야 합니다.
    static bool IsPlaying();
    // 게임을 재개합니다.
    static void Resume();
    // 게임을 정지합니다.
    static void Pause();
private:
    static bool isPlaying;
};
