#include "PlayerPortrait.h"
#include "UIManager.h"
#include "InWanderLand.h"

void PlayerPortraitUIs::SetPortraitHurt(PlayerCharacterType::Enum playerCharType)
{
    if (playerCharType == PlayerCharacterType::None)
        return;
    for (auto each : GetCharInfoUIs(playerCharType))
    {
        each->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->imageComponent.lock()->GetGI().SetVideo(wanderResources::GetPortraitVideoHurt(playerCharType).lock()->GetName());
    };
}

void PlayerPortraitUIs::SetPortraitIdle(PlayerCharacterType::Enum playerCharType)
{
    if (playerCharType == PlayerCharacterType::None)
        return;
    for (auto each : GetCharInfoUIs(playerCharType))
    {
        each->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->imageComponent.lock()->GetGI().SetVideo(wanderResources::GetPortraitVideoIdle(playerCharType).lock()->GetName());
    };
}

void PlayerPortraitUIs::ReflectCooltime(SkillType::Enum skillType, float cooltime, float maxCooltime)
{
    PlayerCharacterType::Enum playerType{ PlayerCharacterType::None };
    switch (skillType)
    {
    case SkillType::ROBIN_Q:
    case SkillType::ROBIN_W:
        playerType = PlayerCharacterType::Robin;
        break;
    case SkillType::URSULA_Q:
    case SkillType::URSULA_W:
        playerType = PlayerCharacterType::Ursula;
        break;
    case SkillType::HANSEL_Q:
    case SkillType::HANSEL_W:
        playerType = PlayerCharacterType::Hansel;
        break;
    }
    for (auto each : GetCharInfoUIs(playerType))
    {
        UIElement* cooltimeNumberUI = nullptr;
        UIElement* cooltimeOverlay = nullptr;
        switch (skillType)
        {
        case SkillType::ROBIN_Q:
        case SkillType::URSULA_Q:
        case SkillType::HANSEL_Q:
            cooltimeNumberUI = each->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_Q_Cooltime_Number);
            cooltimeOverlay = each->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_Q_Overlay);
            break;
        case SkillType::ROBIN_W:
        case SkillType::URSULA_W:
        case SkillType::HANSEL_W:
            cooltimeNumberUI = each->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_W_Cooltime_Number);
            cooltimeOverlay = each->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Skill_Use_W_Overlay);
            break;
        }
        if (cooltimeNumberUI) cooltimeNumberUI->SetNumber(cooltime);
        if (cooltimeOverlay) cooltimeOverlay->adjuster->SetTargetFloat(cooltime / maxCooltime);
    }
}

std::array<UIElement*, 2> PlayerPortraitUIs::GetCharInfoUIs(PlayerCharacterType::Enum playerCharType)
{
    switch (playerCharType)
    {
    case PlayerCharacterType::Robin:
        return { UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin),
            UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin_Left) };
    case PlayerCharacterType::Ursula:
        return { UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula),
            UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula_Left) };
    case PlayerCharacterType::Hansel:
        return { UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel),
            UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel_Left) };
    }
    assert(false);
    return std::array<UIElement*, 2>();
}
