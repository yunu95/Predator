#include "UIVideoPlayer.h"

void UIVideoPlayer::Enable()
{
    if (!videoPlayer2.expired() && !videoPlayer1.expired() && video1Duration > 0)
    {
        StartCoroutine(EnableCoro());
    }
    else
    {
        //videoPlayer1.lock()->ResetVideo();
        uiImage.lock()->GetGI().SetVideo(videoPlayer1.lock()->GetVideo().lock()->GetName());
    }
}

// videoPath2가 존재할때만 실행되는 코루틴
coroutine::Coroutine UIVideoPlayer::EnableCoro()
{
    //videoPlayer1.lock()->ResetVideo();
    uiImage.lock()->GetGI().SetVideo(videoPlayer1.lock()->GetVideo().lock()->GetName());
    co_yield coroutine::WaitForSeconds{ video1Duration, true };
    //videoPlayer2.lock()->ResetVideo();
    uiImage.lock()->GetGI().SetVideo(videoPlayer2.lock()->GetVideo().lock()->GetName());
    co_return;
}
