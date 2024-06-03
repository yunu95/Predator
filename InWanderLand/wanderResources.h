#pragma once
#include "YunutyEngine.h"
#include "UnitAnimationType.h"

namespace wanderResources
{
    yunuGI::IAnimation* GetAnimation(const std::string& fbx, UnitAnimType animType);
    void PushAnimations(graphics::Animator* animator, const std::string& fbx);
}
