#pragma once
#include "YunutyEngine.h"

namespace tests
{
    class ButtonEvent : public yunutyEngine::Component
    {
    public:
        std::unordered_map<yunutyEngine::KeyCode, std::function<void()>> keyPushEvents;
    protected:
        virtual void Update()
        {
            for (auto& keyPushEvent : keyPushEvents)
            {
                if (yunutyEngine::Input::isKeyPushed(keyPushEvent.first))
                {
                    keyPushEvent.second();
                }
            }
        };
    };
}
