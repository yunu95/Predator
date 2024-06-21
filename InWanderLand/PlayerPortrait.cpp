#include "PlayerPortrait.h"
#include "UIManager.h"
#include "InWanderLand.h"

void PlayerPortraitUIs::SetPortraitHurt(PlayerCharacterType::Enum playerCharType)
{
    for (auto each : GetPortraits(playerCharType))
    {
        each->uiVideoPlayer.lock()->videoPlayer1.lock()->SetVideo(wanderResources::GetPortraitVideoHurt(playerCharType));
    };
}

void PlayerPortraitUIs::SetPortraitIdle(PlayerCharacterType::Enum playerCharType)
{
    for (auto each : GetPortraits(playerCharType))
    {
        each->uiVideoPlayer.lock()->videoPlayer1.lock()->SetVideo(wanderResources::GetPortraitVideoIdle(playerCharType));
    };
}

std::array<UIElement*, 2> PlayerPortraitUIs::GetPortraits(PlayerCharacterType::Enum playerCharType)
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
