#pragma once
#include "PlayMusicTimer.h"
#include "UIImage.h"
#include "InWanderLand.h"

void PlayMusicTimer::Init()
{
    isRealtime = true;
    duration = fadeOutTime + fadeInTime;
    onActivation = [=]()
        {
            fadingOut = true;
        };
    onUpdate = [=](float t)
        {
            if (fadingOut && (fadeOutTime == 0 || t >= fadeOutTime / duration))
            {
                fadingOut = false;
                SoundSystem::StopMusic();
                SoundSystem::PlayMusic(musicPath);
            }
            if (fadingOut)
            {
                t = t / (fadeOutTime / duration);
                SoundSystem::SetMusicVolume(1 - t);
            }
            else
            {
                if (fadeInTime == 0)
                {
                    SoundSystem::SetMusicVolume(1);
                    return;
                }
                t = (t - fadeOutTime / duration) / (fadeInTime / duration);
                SoundSystem::SetMusicVolume(t);
            }
        };
    onCompleteFunction = [=]()
        {
            Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
        };
}