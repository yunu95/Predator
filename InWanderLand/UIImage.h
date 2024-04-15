#pragma once
#include "YunutyEngine.h"
#include "ContentsLayer.h"
#include "Application.h"

class UIImage : public yunutyEngine::graphics::UIImage
{
public:
    virtual void Start() override
    {
        application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
        if (GetGameObject()->GetParentGameObject() == nullptr)
            contentsLayer->RegisterToEditorObjectContainer(GetGameObject());
    }
    virtual void Update() override
    {
        OnTransformUpdate();
    }
};