#pragma once

#include "YunutyEngine.h"

class InitialLoadingScreen : public yunutyEngine::Component
{
public:
    virtual void Start() override;
    coroutine::Coroutine ShowLoadingScreen();
};
