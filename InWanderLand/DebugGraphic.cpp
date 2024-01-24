#include "DebugGraphic.h"

unordered_set<graphics::Renderable<yunuGI::IRenderable>*> DebugGraphic::debugGraphics;
bool DebugGraphic::isEnabled{true};

void DebugGraphic::SetEnabled(bool enabled)
{
    isEnabled = enabled;
    for (auto& debugGraphic : debugGraphics)
        debugGraphic->SetActive(enabled);
}
bool DebugGraphic::IsEnabled()
{
    return isEnabled;
}
DebugGraphic::DebugGraphic(graphics::Renderable<yunuGI::IRenderable>* renderable) : renderable(renderable)
{
    debugGraphics.insert(renderable);
    renderable->SetActive(isEnabled);
}
DebugGraphic::~DebugGraphic()
{
    debugGraphics.erase(renderable);
}
