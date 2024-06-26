#include "wanderResources.h"
#include "FBXPool.h"
std::unordered_map<std::string, std::unordered_map<UnitAnimType, yunuGI::IAnimation*>> animMap;
std::unordered_map<std::string, std::unordered_map<yunuGI::IAnimation*, UnitAnimType>> animTypeMap;
std::unordered_map<std::string, std::unordered_map<UnitAnimType, std::string>> fbxMap;
std::unordered_map<std::string, std::string> projectileBirthSounds;
std::unordered_map<std::string, std::string> projectileDeathSounds;

// 온갖 지저분한 애니메이션 상수 키들은 다 여기에 집어넣는다.
void InitAnimMap()
{
    std::unordered_map<std::wstring, yunuGI::IAnimation*> localAnimMap;
    std::transform(graphics::Renderer::SingleInstance().GetResourceManager()->GetAnimationList().begin(),
        graphics::Renderer::SingleInstance().GetResourceManager()->GetAnimationList().end(),
        std::inserter(localAnimMap, localAnimMap.end()), [](auto each) { return std::pair<std::wstring, yunuGI::IAnimation*>{each->GetName(), each}; });
    animMap["SKM_Robin"][UnitAnimType::Idle] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Idle"];
    animMap["SKM_Robin"][UnitAnimType::Move] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Walk"];
    animMap["SKM_Robin"][UnitAnimType::Birth] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Live"];
    animMap["SKM_Robin"][UnitAnimType::BattleIdle] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_BattleIdle"];
    animMap["SKM_Robin"][UnitAnimType::BattleMove] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_BattleWalk"];
    animMap["SKM_Robin"][UnitAnimType::Death] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Death"];
    animMap["SKM_Robin"][UnitAnimType::BattleStart] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_BattleStart"];
    animMap["SKM_Robin"][UnitAnimType::Rush] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Skill1"];
    animMap["SKM_Robin"][UnitAnimType::Attack] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Attack"];
    animMap["SKM_Robin"][UnitAnimType::Taunt] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Skill2"];
    animMap["SKM_Robin"][UnitAnimType::Slam] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Skill1-2"];

    animMap["SKM_Ursula"][UnitAnimType::Attack] = localAnimMap[L"Ani_Ursula_Attack"];
    animMap["SKM_Ursula"][UnitAnimType::Death] = localAnimMap[L"Ani_Ursula_Death"];
    animMap["SKM_Ursula"][UnitAnimType::Idle] = localAnimMap[L"Ani_Ursula_Idle"];
    animMap["SKM_Ursula"][UnitAnimType::BattleIdle] = localAnimMap[L"Ani_Ursula_Idle"];
    animMap["SKM_Ursula"][UnitAnimType::Birth] = localAnimMap[L"Ani_Ursula_Live"];
    animMap["SKM_Ursula"][UnitAnimType::Skill1] = localAnimMap[L"Ani_Ursula_Skill1"];
    animMap["SKM_Ursula"][UnitAnimType::Skill2] = localAnimMap[L"Ani_Ursula_Skill2"];
    animMap["SKM_Ursula"][UnitAnimType::Move] = localAnimMap[L"Ani_Ursula_Walk"];
    animMap["SKM_Ursula"][UnitAnimType::BattleMove] = localAnimMap[L"Ani_Ursula_Walk"];

    animMap["SKM_Hansel"][UnitAnimType::Idle] = localAnimMap[L"Rig_Robin|Ani_Hansel_Idle"];
    animMap["SKM_Hansel"][UnitAnimType::Move] = localAnimMap[L"Rig_Robin|Ani_Hansel_Walk"];
    animMap["SKM_Hansel"][UnitAnimType::Birth] = localAnimMap[L"Rig_Robin|Ani_Hansel_Live"];
    animMap["SKM_Hansel"][UnitAnimType::BattleIdle] = localAnimMap[L"Rig_Robin|Ani_Hansel_Idle"];
    animMap["SKM_Hansel"][UnitAnimType::BattleMove] = localAnimMap[L"Rig_Robin|Ani_Hansel_Walk"];
    animMap["SKM_Hansel"][UnitAnimType::Death] = localAnimMap[L"Rig_Robin|Ani_Hansel_Death"];
    animMap["SKM_Hansel"][UnitAnimType::BattleStart] = localAnimMap[L"Rig_Robin|Ani_Hansel_Live"];
    animMap["SKM_Hansel"][UnitAnimType::Rush] = localAnimMap[L"Rig_Robin|Ani_Hansel_Walk"];
    animMap["SKM_Hansel"][UnitAnimType::Attack] = localAnimMap[L"Rig_Robin|Ani_Hansel_Attack "];
    animMap["SKM_Hansel"][UnitAnimType::Taunt] = localAnimMap[L"Rig_Robin|Ani_Hansel_Skill2"];
    animMap["SKM_Hansel"][UnitAnimType::Slam] = localAnimMap[L"Rig_Robin|Ani_Hansel_Attack "];
    animMap["SKM_Hansel"][UnitAnimType::Skill1] = localAnimMap[L"Rig_Robin|Ani_Hansel_Skill1"];
    animMap["SKM_Hansel"][UnitAnimType::Throw] = localAnimMap[L"Rig_Robin|Ani_Hansel_Skill2"];

    animMap["SKM_Monster1"][UnitAnimType::Slam] = localAnimMap[L"Ani_Monster1_Attack"];
    animMap["SKM_Monster1"][UnitAnimType::Attack] = localAnimMap[L"Ani_Monster1_Attack"];
    animMap["SKM_Monster1"][UnitAnimType::Skill1] = localAnimMap[L"Ani_Monster1_Skill"];
    animMap["SKM_Monster1"][UnitAnimType::Skill2] = localAnimMap[L"Ani_Monster1_Skill"];
    animMap["SKM_Monster1"][UnitAnimType::Idle] = localAnimMap[L"Ani_Monster1_Idle"];
    animMap["SKM_Monster1"][UnitAnimType::BattleIdle] = localAnimMap[L"Ani_Monster1_Idle"];
    animMap["SKM_Monster1"][UnitAnimType::Paralysis] = localAnimMap[L"Ani_Monster1_Stun"];
    animMap["SKM_Monster1"][UnitAnimType::Move] = localAnimMap[L"Ani_Monster1_Walk"];
    animMap["SKM_Monster1"][UnitAnimType::BattleMove] = localAnimMap[L"Ani_Monster1_Walk"];
    animMap["SKM_Monster1"][UnitAnimType::Death] = localAnimMap[L"Ani_Monster1_Death"];
    animMap["SKM_Monster1"][UnitAnimType::Spin] = localAnimMap[L"Ani_Monster1_Skill"];

    animMap["SKM_Monster2"][UnitAnimType::Attack] = localAnimMap[L"Ani_Monster2_Attack"];
    animMap["SKM_Monster2"][UnitAnimType::Paralysis] = localAnimMap[L"Ani_Monster2_HitCC"];
    animMap["SKM_Monster2"][UnitAnimType::Idle] = localAnimMap[L"Ani_Monster2_Idle"];
    animMap["SKM_Monster2"][UnitAnimType::BattleIdle] = localAnimMap[L"Ani_Monster2_Idle"];
    animMap["SKM_Monster2"][UnitAnimType::Slam] = localAnimMap[L"Ani_Monster2_Skill"];
    animMap["SKM_Monster2"][UnitAnimType::Skill1] = localAnimMap[L"Ani_Monster2_Skill"];
    animMap["SKM_Monster2"][UnitAnimType::Skill2] = localAnimMap[L"Ani_Monster2_Skill"];
    animMap["SKM_Monster2"][UnitAnimType::Move] = localAnimMap[L"Ani_Monster2_Walk"];
    animMap["SKM_Monster2"][UnitAnimType::BattleMove] = localAnimMap[L"Ani_Monster2_Walk"];
    animMap["SKM_Monster2"][UnitAnimType::Death] = localAnimMap[L"Ani_Monster2_Death"];

    animMap["SKM_HeartQueen"][UnitAnimType::Attack] = localAnimMap[L"Ani_HeartQueen_Attack"];
    animMap["SKM_HeartQueen"][UnitAnimType::Death] = localAnimMap[L"Ani_HeartQueen_Death"];
    animMap["SKM_HeartQueen"][UnitAnimType::Idle] = localAnimMap[L"Ani_HeartQueen_Idle"];
    animMap["SKM_HeartQueen"][UnitAnimType::BattleIdle] = localAnimMap[L"Ani_HeartQueen_Idle"];
    animMap["SKM_HeartQueen"][UnitAnimType::Skill1] = localAnimMap[L"Ani_HeartQueen_Skill1"];
    animMap["SKM_HeartQueen"][UnitAnimType::Skill2] = localAnimMap[L"Ani_HeartQueen_Skill2"];
    animMap["SKM_HeartQueen"][UnitAnimType::Skill3] = localAnimMap[L"Ani_HeartQueen_Skill3"];
    animMap["SKM_HeartQueen"][UnitAnimType::Skill4] = localAnimMap[L"Ani_HeartQueen_Skill3"];
    animMap["SKM_HeartQueen"][UnitAnimType::BattleMove] = localAnimMap[L"Ani_HeartQueen_Walk"];
    animMap["SKM_HeartQueen"][UnitAnimType::Move] = localAnimMap[L"Ani_HeartQueen_Walk"];
    animMap["SKM_HeartQueen"][UnitAnimType::Birth] = localAnimMap[L"Ani_HeartQueen_Appear"];

    animMap["SKM_Frame1"][UnitAnimType::Birth] = localAnimMap[L"Ani_Frame1_Appear"];
    animMap["SKM_Frame1"][UnitAnimType::Idle] = localAnimMap[L"Ani_Frame1_Idle"];
    animMap["SKM_Frame1"][UnitAnimType::Death] = localAnimMap[L"Ani_Frame1_Death"];
    animMap["SKM_Frame1"][UnitAnimType::Skill1] = localAnimMap[L"Ani_Frame1_Summon"];

    animMap["SKM_Frame2"][UnitAnimType::Birth] = localAnimMap[L"Ani_Frame2_Appear"];
    animMap["SKM_Frame2"][UnitAnimType::Idle] = localAnimMap[L"Ani_Frame2_Idle"];
    animMap["SKM_Frame2"][UnitAnimType::Death] = localAnimMap[L"Ani_Frame2_Death"];
    animMap["SKM_Frame2"][UnitAnimType::Skill1] = localAnimMap[L"Ani_Frame2_Summon"];

    // 없는 애니메이션들은 전부 Idle로 대체한다.
    for (auto& each : animMap)
    {
        for (auto enumItr = (int)UnitAnimType::None + 1; enumItr != (int)UnitAnimType::End; enumItr++)
        {
            UnitAnimType animType = (UnitAnimType)enumItr;
            if (!each.second.contains(animType))
            {
                each.second[animType] = each.second[UnitAnimType::Idle];
            }
        }
    }

    for (auto& [fbx, map] : animMap)
    {
        for (auto& [type, ani] : map)
        {
            animTypeMap[fbx][ani] = type;
        }
    }
};

yunuGI::IAnimation* wanderResources::GetAnimation(const std::string& fbx, UnitAnimType animType)
{
    if (animMap.empty())
        InitAnimMap();
    return animMap.at(fbx).at(animType);
}

UnitAnimType wanderResources::GetAnimationType(const std::string& fbx, yunuGI::IAnimation* animation)
{
    if (animMap.empty())
    {
        InitAnimMap();
    }

    if (!animTypeMap.contains(fbx) || !animTypeMap[fbx].contains(animation))
    {
        return UnitAnimType::None;
    }

    return animTypeMap.at(fbx).at(animation);
}

constexpr const std::string wanderResources::GetFBXName(WanderFBX::Enum fbxType)
{
    switch (fbxType)
    {
    case wanderResources::WanderFBX::None: return "";
    case wanderResources::WanderFBX::IMPALING_SPIKE: return "Spear";
    case wanderResources::WanderFBX::HEALING_COOKIE: return "VFX_HealPack";
    case wanderResources::WanderFBX::BOSS_SPIKE: return "SM_Spike02";
    case wanderResources::WanderFBX::NUM: return "";
    default:
        break;
    }
}

std::weak_ptr<yunuGI::IVideo> wanderResources::GetPortraitVideoIdle(PlayerCharacterType::Enum playerType)
{
    auto rsrc = graphics::Renderer::SingleInstance().GetResourceManager();
    switch (playerType)
    {
    case PlayerCharacterType::Robin: return rsrc->GetVideoData(L"Texture/Ingame/NewFolder/Portrait_Robin_Idle.mp4");
    case PlayerCharacterType::Ursula: return rsrc->GetVideoData(L"Texture/Ingame/NewFolder/Portrait_Ursula_Idle.mp4");
    case PlayerCharacterType::Hansel: return rsrc->GetVideoData(L"Texture/Ingame/NewFolder/Portrait_Hansel_Idle.mp4");
    }
    return std::weak_ptr<yunuGI::IVideo>{};
}

std::weak_ptr<yunuGI::IVideo> wanderResources::GetPortraitVideoHurt(PlayerCharacterType::Enum playerType)
{
    auto rsrc = graphics::Renderer::SingleInstance().GetResourceManager();
    switch (playerType)
    {
    case PlayerCharacterType::Robin: return rsrc->GetVideoData(L"Texture/Ingame/NewFolder/Portrait_Robin_Hurt.mp4");
    case PlayerCharacterType::Ursula: return rsrc->GetVideoData(L"Texture/Ingame/NewFolder/Portrait_Ursula_Hurt.mp4");
    case PlayerCharacterType::Hansel: return rsrc->GetVideoData(L"Texture/Ingame/NewFolder/Portrait_Hansel_Hurt.mp4");
    }
    return std::weak_ptr<yunuGI::IVideo>{};
}

void InitFBXMap()
{
    fbxMap["SKM_Robin"][UnitAnimType::Attack] = "VFX_Robin_Attack";
    fbxMap["SKM_Ursula"][UnitAnimType::Attack] = "VFX_Ursula_Attack_1";
    fbxMap["SKM_Hansel"][UnitAnimType::Attack] = "VFX_Hansel_Attack_1";
    fbxMap["SKM_Monster1"][UnitAnimType::Attack] = "VFX_Monster1_Attack";
    fbxMap["SKM_HeartQueen"][UnitAnimType::Attack] = "VFX_HeartQueen_Attack";

    fbxMap["SKM_Ursula"][UnitAnimType::Damaged] = "VFX_Ursula_Attack_2";
    fbxMap["SKM_Hansel"][UnitAnimType::Damaged] = "VFX_Hansel_Attack_2";
}

std::weak_ptr<ManagedFBX> wanderResources::GetVFX(const std::string& fbx, UnitAnimType animType)
{
    if (fbxMap.empty())
        InitFBXMap();
    return FBXPool::Instance().Borrow(fbxMap.at(fbx).at(animType));
}

bool wanderResources::FindVFXMap(const std::string& fbx, UnitAnimType animType)
{
    auto itr = fbxMap.find(fbx);

    if (itr == fbxMap.end() || itr->second.find(animType) == itr->second.end())
    {
        return false;
    }
    else
    {
        return true;
    }
}
