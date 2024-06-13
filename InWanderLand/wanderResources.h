#pragma once
#include "YunutyEngine.h"
#include "WanderFBX.h"
#include "UnitAnimationType.h"

class ManagedFBX;
namespace wanderResources
{
    // 온갖 지저분한 애니메이션 상수들은 다 여기에 집어넣는다.
    yunuGI::IAnimation* GetAnimation(const std::string& fbx, UnitAnimType animType);
    constexpr const std::string GetFBXName(WanderFBX::Enum fbxType);

    std::weak_ptr<ManagedFBX> GetVFX(const std::string& fbx, UnitAnimType animType);

    namespace texture
    {
        const std::wstring MAX_SKILL_RANGE_TEXTURE{ L"Texture/T_VFX_SkillRange_Cicle.dds" };
        const std::wstring MOVE_TEXTURE{ L"Texture/move.png" };
        const std::wstring ARROW_HEAD_TEXTURE{ L"Texture/T_VFX_SkillLengthFinish.dds" };
        const std::wstring ARROW_BODY_TEXTURE{ L"Texture/T_VFX_SkillLength.dds" };
        const std::wstring ROBIN_W_TEXTURE{ L"Texture/T_VFX_SkillRange_Clock.dds" };
        const std::wstring URSULA_Q_TEXTURE{ L"Texture/T_VFX_SkillRange_Clock.dds" };
        const std::wstring URSULA_W_TEXTURE{ L"Texture/T_VFX_SkillRange_Clock.dds" };
        const std::wstring HANSEL_Q_TEXTURE{ L"Texture/T_VFX_SkillRange_Clock.dds" };
        const std::wstring MOVE_END_TEXTURE{ L"Texture/T_VFX_SkillRange_Move.dds" };
        const std::wstring ATTACK_TEXTURE{ L"Texture/T_VFX_AttackCircle.dds" };
    }
    namespace unitColor
    {
        const yunuGI::Color ROBIN_COLOR{ 1,0,0,1 };
        const yunuGI::Color URSULA_COLOR{ 0.545,0,1,1 };
        const yunuGI::Color HANSEL_COLOR{ 1,0.5,0,1 };
        const yunuGI::Color ENEMY_COLOR{ 0,1,1,1 };
    }
}
