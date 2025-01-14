#pragma once
#include "YunutyEngine.h"
#include "JsonUIData.h"
#include "UIImage.h"
#include "UIEnumID.h"
#include "RotatingUI.h"
#include "UIVideoPlayer.h"

class LinearClippingTimer;
class UIButton;
class FloatFollower;
class SoundPlayingTimer;
class UIPriorityLayout;
class PopDownOnDisable;
class PopupOnEnable;
class TimePauseTimer;
class UIOffsetTransition;
class ColorTintTimer;
class PlayMusicTimer;
class UISpriteAnimation;
class TimerComponent;
// 임포트된 UI 요소에 대한 정보를 잔뜩 저장하는 클래스
class UIElement : public Component
{
public:
    JsonUIData importedUIData;
private:
    // 복제된 UIElement의 하위 요소들에 대한 정보
    int uiPriority{ 0 };
    std::unordered_map<int, UIElement*> localUIsByIndex;
    std::unordered_map<UIEnumID, UIElement*> localUIsByEnumID;
    std::unordered_map<int, JsonUIData> localUIdatasByIndex;
    bool enabled = true;
    //virtual Component* GetComponent() override { return this; }
public:
    bool GetUIEnabled() { return enabled; }
    virtual void Start() override;
    virtual void Update() override;
    vector<UIElement*> disablePropagationTargets;
    vector<UIElement*> enablePropagationTargets;
    int duplicatePriorityOffset = 0;
    PopupOnEnable* scalePopUpTransition{ nullptr };
    PopDownOnDisable* scalePopDownTransition{ nullptr };
    UIOffsetTransition* enableTransition{ nullptr };
    UIOffsetTransition* disableTransition{ nullptr };
    ColorTintTimer* colorTintOnEnable{ nullptr };
    ColorTintTimer* colorTintOnDisable{ nullptr };
    TimePauseTimer* timePauseOnEnable{ nullptr };
    SoundPlayingTimer* soundOnClick{ nullptr };
    SoundPlayingTimer* soundOnHover{ nullptr };
    SoundPlayingTimer* soundOnEnable{ nullptr };
    SoundPlayingTimer* soundOnDisable{ nullptr };
    UISpriteAnimation* spriteAnimationOnEnable{ nullptr };
    TimerComponent* disableAfterEnable{ nullptr };
    TimerComponent* lerpMusicVolumeOnEnable{ nullptr };
    TimerComponent* lerpMusicVolumeOnDisable{ nullptr };
    std::weak_ptr<graphics::UIImage> imageComponent{};
    std::weak_ptr<RotatingUI> rotator{};
    std::weak_ptr<UIVideoPlayer> uiVideoPlayer{};
    graphics::UIText* textComponent{ nullptr };
    UIPriorityLayout* priorityLayout{ nullptr };
    UIPriorityLayout* parentPriorityLayout{ nullptr };
    UIButton* button{ nullptr };
    LinearClippingTimer* linearClippingTimerOnEnable{ nullptr };
    LinearClippingTimer* linearClippingTimerOnDisable{ nullptr };
    vector<UIElement*> children;
    vector<UIElement*> exclusiveEnableGroup;
    unsigned int runtimeFlags{ 0 };
    // PropagateEnable을 막기 위한  똥꼬쇼 변수
    bool reallyDisabled{ false };
    void EnableElement();
    void DisableElement();
    void DisableElementInstant();
    void SetNumber(float number);
    const std::unordered_map<int, UIElement*>& GetLocalUIsByIndex() { return localUIsByIndex; };
    const std::unordered_map<UIEnumID, UIElement*>& GetLocalUIsByEnumID() { return localUIsByEnumID; };
    const std::unordered_map<int, JsonUIData>& GetLocalUIdatasByIndex() { return localUIdatasByIndex; };
    // UI 요소에 영향을 줄 수 있는 실수 값을 조정합니다.
    FloatFollower* adjuster{ nullptr };
    UIEnumID GetDuplicateParentEnumID() { return duplicateParentEnumID; }

    void SetAsWorldSpaceUI(const Vector3d& origin);
    void SetAsScreenSpaceUI();

    //virtual void PlayFunction() override;
    //virtual void StopFunction() override;

private:
    UIEnumID duplicateParentEnumID{ UIEnumID::None };
    bool numberSetBefore = false;
    // 0~9까지의 숫자 이미지를 저장하는 배열
    array<yunuGI::ITexture*, 10>* digitFont{ };
    // 10^i 자리에 속하는 숫자의 이미지를 저장하는 배열
    std::vector<UIElement*> digits{ };
    Transform* worldParent{ nullptr };
    Vector3d worldParentOrigin;
    friend class UIManager;
};
