#pragma once
#include "YunutyEngine.h"

class DebugGraphic
{
public:
    static void SetEnabled(bool enabled);
    static bool IsEnabled();
    DebugGraphic(graphics::Renderable<yunuGI::IRenderable>* renderable);
    virtual ~DebugGraphic();
private:
    static bool isEnabled;
    static unordered_set<graphics::Renderable<yunuGI::IRenderable>*> debugGraphics;
    graphics::Renderable<yunuGI::IRenderable>* renderable;
};
