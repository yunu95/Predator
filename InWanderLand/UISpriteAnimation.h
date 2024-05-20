#pragma once
#include "SpriteAnimation.h"
#include "YunutyEngine.h"
#include "UICurveType.h"
#include "Timer.h"

struct JsonUIData;
class UIElement;
class UIManager;
// 스프라이트 애니메이션을 재생하는 컴포넌트
class UISpriteAnimation : public TimerComponent
{
private:
    int currentIdx{ 0 };
    UIElement* uiElement{ nullptr };
public:
    std::weak_ptr<graphics::UIImage> imageComponent{};
    const std::vector<SpriteAnimation::Sprite>* spriteSheet{ nullptr };
    void SetSprites(const wchar_t* spritesRootPath);
    void Init();
    friend UIManager;
};
