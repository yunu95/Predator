#include "VideoPlayer.h"

namespace yunutyEngine::graphics
{
    void VideoPlayer::Update()
    {
        if (auto gi = video.lock(); gi)
        {
            gi->update(usingUnscaledTime?Time::GetDeltaTimeUnscaled():Time::GetDeltaTime(), loop);
        }
    }
}