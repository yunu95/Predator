#pragma once
#include "PlayMusicTimer.h"
#include "UIImage.h"
#include "InWanderLand.h"

void PlayMusicTimer::Init()
{
    isRealtime = true;
    pushDuration = fadeOutTime + fadeInTime;
    onActivation = [=]()
        {
            fadingOut = true;
        };
    onUpdate = [=](float t)
        {
            if (fadingOut && (fadeOutTime == 0 || t >= fadeOutTime / pushDuration))
            {
                fadingOut = false;
                SoundSystem::StopMusic();
                SoundSystem::PlayMusic(musicPath);
            }
            if (fadingOut)
            {
                t = t / (fadeOutTime / pushDuration);
                SoundSystem::SetMusicVolume(1 - t);
            }
            else
            {
                if (fadeInTime == 0)
                {
                    SoundSystem::SetMusicVolume(1);
                    return;
                }
                t = (t - fadeOutTime / pushDuration) / (fadeInTime / pushDuration);
                SoundSystem::SetMusicVolume(t);
            }
        };
    onCompleteFunction = [=]()
        {
            Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
        };
}