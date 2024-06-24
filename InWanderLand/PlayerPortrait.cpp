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
