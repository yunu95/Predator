#include "DebugGraphic.h"

unordered_set<graphics::Renderable<yunuGI::IRenderable>*> DebugGraphic::debugGraphics;
#ifdef EDITOR
bool DebugGraphic::isEnabled{ true };
#else
bool DebugGraphic::isEnabled{ false };
#endif

void DebugGraphic::SetDebugGraphicsEnabled(bool enabled)
{
    isEnabled = enabled;
    for (auto& debugGraphic : debugGraphics)
        debugGraphic->SetActive(enabled);
}
bool DebugGraphic::AreDebugGraphicsEnabled()
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
