#pragma once
#include "YunutyEngine.h"
#include "WanderFBX.h"
#include "UnitAnimationType.h"

namespace wanderResources
{
    // 온갖 지저분한 애니메이션 상수들은 다 여기에 집어넣는다.
    yunuGI::IAnimation* GetAnimation(const std::string& fbx, UnitAnimType animType);
    constexpr const std::string GetFBXName(WanderFBX::Enum fbxType);
}
