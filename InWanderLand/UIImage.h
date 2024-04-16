#pragma once
#include "YunutyEngine.h"
#include "ContentsLayer.h"
#include "Application.h"

class UIImage : public yunutyEngine::graphics::UIImage
{
public:
    
    virtual void Update() override
    {
        OnTransformUpdate();
    }
};