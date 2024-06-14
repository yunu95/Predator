#pragma once
#include "Object.h"
#include "Component.h"
#include "Rect.h"
#include "YunutyTime.h"
#include "YunuGraphicsInterface.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine::graphics
{
    class YUNUTY_API VideoPlayer : public Component
    {
    protected:
        std::weak_ptr<yunuGI::IVideo> video;
    public:
        void SetVideo(std::weak_ptr<yunuGI::IVideo> video)
        {
            this->video = video;
        };
        std::weak_ptr<yunuGI::IVideo> GetVideo()
        {
            return video;
        };
        void ResetVideo()
        {
            video.lock()->ResetVideo();
        };
        virtual void Update() override;
        bool usingUnscaledTime{ true };
        bool loop{ true };
    };
}
