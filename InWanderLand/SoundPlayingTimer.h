#pragma once
#include "Timer.h"
#include "SFXManager.h"

class SoundPlayingTimer : public TimerComponent
{
public:
    std::string soundPath;
    void Init()
    {
        isRealtime = true;
        onCompleteFunction = [this]()
            {
                // 사운드를 재생합니다.
                SFXManager::PlaySoundfile(soundPath);
            };
    }
};
