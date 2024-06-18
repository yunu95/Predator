#include "ContentsCoroutine.h"

std::weak_ptr<coroutine::Coroutine> ContentsCoroutine::StartRoutine(coroutine::Coroutine&& coroutine)
{
    return Instance().StartCoroutine(std::move(coroutine));
}

Component* ContentsCoroutine::GetComponent()
{
    return this;
}

std::weak_ptr<coroutine::Coroutine> ContentsCoroutine::FadeMusicVolume(float targetVolume, float duration)
{
    return Instance().StartCoroutine(FadeMusicVolumeInternal(targetVolume, duration));
}

coroutine::Coroutine ContentsCoroutine::FadeMusicVolumeInternal(float targetVolume, float duration)
{
    coroutine::ForSeconds forSeconds{ duration ,true };
    float startVolume = SoundSystem::GetMusicVolume();
    while (forSeconds.Tick())
    {
        SoundSystem::SetMusicVolume(math::LerpF(startVolume, targetVolume, forSeconds.ElapsedNormalized()));
        co_await std::suspend_always{};
    }
    co_return;
}

