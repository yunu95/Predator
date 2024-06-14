#pragma once
#include "YunutyEngine.h"

class UIVideoPlayer : public Component
{
public:
    std::weak_ptr<graphics::UIImage> uiImage;
    std::weak_ptr<graphics::VideoPlayer> videoPlayer1;
    float video1Duration{ 0 };
    std::weak_ptr<graphics::VideoPlayer> videoPlayer2;
    void Enable();
private:
    coroutine::Coroutine EnableCoro();
};