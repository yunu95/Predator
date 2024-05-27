#include "wanderResources.h"

std::unordered_map<std::wstring, std::unordered_map<UnitAnimType, yunuGI::IAnimation*>> animMap;
std::unordered_map<std::wstring, std::string> projectileBirthSounds;
std::unordered_map<std::wstring, std::string> projectileDeathSounds;

// 온갖 지저분한 애니메이션 상수 키들은 다 여기에 집어넣는다.
void InitAnimMap()
{
    std::unordered_map<std::wstring, yunuGI::IAnimation*> localAnimMap;
    std::transform(graphics::Renderer::SingleInstance().GetResourceManager()->GetAnimationList().begin(),
        graphics::Renderer::SingleInstance().GetResourceManager()->GetAnimationList().end(),
        std::inserter(localAnimMap, localAnimMap.end()), [](auto each) {std::pair<std::wstring, yunuGI::IAnimation*>{each->GetName(), each}; });
    animMap[L"SKM_Robin"][UnitAnimType::Idle] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Idle"];
    animMap[L"SKM_Robin"][UnitAnimType::Move] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Walk"];
    animMap[L"SKM_Robin"][UnitAnimType::Birth] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Live"];
    animMap[L"SKM_Robin"][UnitAnimType::BattleIdle] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_BattleIdle"];
    animMap[L"SKM_Robin"][UnitAnimType::BattleMove] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_BattleWalk"];
    animMap[L"SKM_Robin"][UnitAnimType::Death] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Death"];
    animMap[L"SKM_Robin"][UnitAnimType::BattleStart] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_BattleStart"];
    animMap[L"SKM_Robin"][UnitAnimType::Rush] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Skill1"];
    animMap[L"SKM_Robin"][UnitAnimType::Attack] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Attack"];
    animMap[L"SKM_Robin"][UnitAnimType::Taunt] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Skill2"];
    animMap[L"SKM_Robin"][UnitAnimType::Slam] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Attack"];

    animMap[L"SKM_Ursula"][UnitAnimType::Attack] = localAnimMap[L"Ani_Ursula_Attack"];
    animMap[L"SKM_Ursula"][UnitAnimType::Death] = localAnimMap[L"Ani_Ursula_Death"];
    animMap[L"SKM_Ursula"][UnitAnimType::Idle] = localAnimMap[L"Ani_Ursula_Idle"];
    animMap[L"SKM_Ursula"][UnitAnimType::BattleIdle] = localAnimMap[L"Ani_Ursula_Idle"];
    animMap[L"SKM_Ursula"][UnitAnimType::Birth] = localAnimMap[L"Ani_Ursula_Live"];
    animMap[L"SKM_Ursula"][UnitAnimType::Skill1] = localAnimMap[L"Ani_Ursula_Skill1"];
    animMap[L"SKM_Ursula"][UnitAnimType::Skill2] = localAnimMap[L"Ani_Ursula_Skill2"];
    animMap[L"SKM_Ursula"][UnitAnimType::Move] = localAnimMap[L"Ani_Ursula_Walk"];
    animMap[L"SKM_Ursula"][UnitAnimType::BattleMove] = localAnimMap[L"Ani_Ursula_Walk"];

    animMap[L"SKM_Hansel"][UnitAnimType::Idle] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Idle"];
    animMap[L"SKM_Hansel"][UnitAnimType::Move] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Walk"];
    animMap[L"SKM_Hansel"][UnitAnimType::Birth] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Live"];
    animMap[L"SKM_Hansel"][UnitAnimType::BattleIdle] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_BattleIdle"];
    animMap[L"SKM_Hansel"][UnitAnimType::BattleMove] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_BattleWalk"];
    animMap[L"SKM_Hansel"][UnitAnimType::Death] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Death"];
    animMap[L"SKM_Hansel"][UnitAnimType::BattleStart] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_BattleStart"];
    animMap[L"SKM_Hansel"][UnitAnimType::Rush] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Skill1"];
    animMap[L"SKM_Hansel"][UnitAnimType::Attack] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Attack"];
    animMap[L"SKM_Hansel"][UnitAnimType::Taunt] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Skill2"];
    animMap[L"SKM_Hansel"][UnitAnimType::Slam] = localAnimMap[L"Rig_Robin_arpbob|Ani_Robin_Attack"];

    animMap[L"SKM_Monster1"][UnitAnimType::Slam] = localAnimMap[L"Ani_Monster1_Attack"];
    animMap[L"SKM_Monster1"][UnitAnimType::Attack] = localAnimMap[L"Ani_Monster1_Attack"];
    animMap[L"SKM_Monster1"][UnitAnimType::Skill1] = localAnimMap[L"Ani_Monster1_Skill"];
    animMap[L"SKM_Monster1"][UnitAnimType::Skill2] = localAnimMap[L"Ani_Monster1_Skill"];
    animMap[L"SKM_Monster1"][UnitAnimType::Idle] = localAnimMap[L"Ani_Monster1_Idle"];
    animMap[L"SKM_Monster1"][UnitAnimType::BattleIdle] = localAnimMap[L"Ani_Monster1_Idle"];
    animMap[L"SKM_Monster1"][UnitAnimType::Paralysis] = localAnimMap[L"Ani_Monster1_Stun"];
    animMap[L"SKM_Monster1"][UnitAnimType::Move] = localAnimMap[L"Ani_Monster1_Walk"];
    animMap[L"SKM_Monster1"][UnitAnimType::BattleMove] = localAnimMap[L"Ani_Monster1_Walk"];

    animMap[L"SKM_Monster2"][UnitAnimType::Attack] = localAnimMap[L"Ani_Monster2_Attack"];
    animMap[L"SKM_Monster2"][UnitAnimType::Paralysis] = localAnimMap[L"Ani_Monster2_HitCC"];
    animMap[L"SKM_Monster2"][UnitAnimType::Idle] = localAnimMap[L"Ani_Monster2_Idle"];
    animMap[L"SKM_Monster2"][UnitAnimType::BattleIdle] = localAnimMap[L"Ani_Monster2_Idle"];
    animMap[L"SKM_Monster2"][UnitAnimType::Slam] = localAnimMap[L"Ani_Monster2_Skill"];
    animMap[L"SKM_Monster2"][UnitAnimType::Skill1] = localAnimMap[L"Ani_Monster2_Skill"];
    animMap[L"SKM_Monster2"][UnitAnimType::Skill2] = localAnimMap[L"Ani_Monster2_Skill"];
    animMap[L"SKM_Monster2"][UnitAnimType::Move] = localAnimMap[L"Ani_Monster2_Walk"];
    animMap[L"SKM_Monster2"][UnitAnimType::BattleMove] = localAnimMap[L"Ani_Monster2_Walk"];

    animMap[L"SKM_HeartQueen"][UnitAnimType::Attack] = localAnimMap[L"Ani_HeartQueen_Attack"];
    animMap[L"SKM_HeartQueen"][UnitAnimType::Death] = localAnimMap[L"Ani_HeartQueen_Death"];
    animMap[L"SKM_HeartQueen"][UnitAnimType::Idle] = localAnimMap[L"Ani_HeartQueen_Idle"];
    animMap[L"SKM_HeartQueen"][UnitAnimType::BattleIdle] = localAnimMap[L"Ani_HeartQueen_Idle"];
    animMap[L"SKM_HeartQueen"][UnitAnimType::Skill1] = localAnimMap[L"Ani_HeartQueen_Skill1"];
    animMap[L"SKM_HeartQueen"][UnitAnimType::Skill2] = localAnimMap[L"Ani_HeartQueen_Skill2"];
    animMap[L"SKM_HeartQueen"][UnitAnimType::Skill3] = localAnimMap[L"Ani_HeartQueen_Skill3"];
    animMap[L"SKM_HeartQueen"][UnitAnimType::Skill4] = localAnimMap[L"Ani_HeartQueen_Skill4"];
    animMap[L"SKM_HeartQueen"][UnitAnimType::BattleMove] = localAnimMap[L"Ani_HeartQueen_Walk"];
    animMap[L"SKM_HeartQueen"][UnitAnimType::Move] = localAnimMap[L"Ani_HeartQueen_Walk"];
    // 없는 애니메이션들은 전부 Idle로 대체한다.
    for (auto& each : animMap)
    {
        for (auto enumItr = (int)UnitAnimType::None + 1; enumItr != (int)UnitAnimType::End; enumItr++)
        {
            UnitAnimType animType = (UnitAnimType)enumItr;
            if (each.second.contains(animType))
            {
                each.second[animType] = each.second[UnitAnimType::Idle];
            }
        }
    }
};
yunuGI::IAnimation* wanderResources::GetAnimation(const char* fbx, UnitAnimType animType)
{
    if (animMap.empty())
        InitAnimMap();
    return nullptr;
}
