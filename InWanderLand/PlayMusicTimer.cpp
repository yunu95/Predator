#pragma once
#include "PlayMusicTimer.h"
#include "UIImage.h"
#include "InWanderLand.h"

void PlayMusicTimer::Init()
{
    isRealtime = true;
    m_duration = fadeOutTime + fadeInTime;
    onActivation = [=]()
        {
            fadingOut = true;
        };
    onUpdate = [=](float t)
        {
            if (fadingOut && (fadeOutTime == 0 || t >= fadeOutTime / m_duration))
            {
                fadingOut = false;
                SoundSystem::StopMusic();
                SoundSystem::PlayMusic(musicPath);
            }
            if (fadingOut)
            {
                t = t / (fadeOutTime / m_duration);
                SoundSystem::SetMusicVolume(1 - t);
            }
            else
            {
                if (fadeInTime == 0)
                {
                    SoundSystem::SetMusicVolume(1);
                    return;
                }
                t = (t - fadeOutTime / m_duration) / (fadeInTime / m_duration);
                SoundSystem::SetMusicVolume(t);
            }
        };
    onCompleteFunction = [=]()
        {
            Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
        };
}