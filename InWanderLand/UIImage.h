#pragma once
#include "YunutyEngine.h"
#include "ContentsLayer.h"
#include "Application.h"

class UIImage : public yunutyEngine::graphics::UIImage
{
public:
    static constexpr int priority_Tooltip = 123456;
    static constexpr int priority_Mask = 1234567;
    virtual void Update() override
    {
        OnTransformUpdate();
    }
};