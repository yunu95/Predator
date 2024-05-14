#pragma once
#include "SpriteAnimation.h"
#include "YunutyEngine.h"
#include "UICurveType.h"
#include "Timer.h"

struct JsonUIData;
class UIElement;
// 게임오브젝트가 활성화될때 스케일을 0에서 확 늘리는 컴포넌트
class UISpriteAnimation : public TimerComponent
{
private:
    int currentIdx{ 0 };
public:
    UIElement* uiElement{ nullptr };
    const std::vector<SpriteAnimation::Sprite>* spriteSheet{ nullptr };
    void SetSprites(const wchar_t* spritesRootPath);
    void Init();
};
