#pragma once
#include "YunutyEngine.h"
#include "PlayerCharacterType.h"

class UIElement;
class PlayerPortraitUIs
{
public:
    static void SetPortraitHurt(PlayerCharacterType::Enum playerCharType);
    static void SetPortraitIdle(PlayerCharacterType::Enum playerCharType);
    static void ReflectCooltime(SkillType::Enum skillType, float cooltime, float maxCooltime);
private:
    static std::array<UIElement*, 2> GetCharInfoUIs(PlayerCharacterType::Enum playerCharType);
    PlayerPortraitUIs() {};
};