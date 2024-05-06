#pragma once
#include "YunutyEngine.h"
#include "InputManager.h"
#include "UIPanel.h"
#include "ContentsObservee.h"

/// <summary>
/// UI Button 클래스.
/// 클릭 시 호출되는 함수(m_ClickedEventFunction)를 정의할 경우 버튼이 됩니다.
/// SetCloseButton() 함수로 닫는 창을 활성화 시킬 수 있습니다.
/// </summary>
class UIButton : public Component
{
private:
    yunuGI::ITexture* m_IdleImage;
    yunuGI::ITexture* m_ClickedImage;

    yunuGI::ITexture* m_CurrentImage;

    std::function<void()> m_onMouseFunction;
    std::function<void()> m_onMouseExitFunction;
    std::function<void()> m_mousePushedFunction;
    std::function<void()> m_mouseLiftedFunction;

    std::function<void()> m_mouseLiftedEventFunction;
    std::vector<std::function<void()>> m_mouseLiftedEventFunctions;
    std::vector<std::function<void()>> m_OnMouseEventFunctions;

    yunutyEngine::graphics::UIImage* m_ImageComponent;

    double m_Width;
    double m_Height;

    Vector2d m_ImageCenterPostion;

    bool isMouseNowOnButton;
    //int m_layer;

    LONG initialRectRight;
    LONG initialRectBottom;

public:
    void SetIdleImage(yunuGI::ITexture* p_IdleImage);
    void SetClickedImage(yunuGI::ITexture* p_ClickedImage);

    void SetButtonClickFunction(std::function<void()> p_func);
    void AddButtonClickFunction(std::function<void()> p_func);
    void AddButtonOnMouseFunction(std::function<void()> p_func);

    void SetLayer(int p_layerNum);
    int GetLayer() const;

    void SetImageComponent(yunutyEngine::graphics::UIImage* p_ImageComponent);

    virtual void Start() override;
    virtual void OnTransformUpdate() override;
    virtual void Update() override;
    virtual void OnEnable() override;
    virtual void OnDisable() override;
    virtual void InvokeButtonClickEvent();

    //virtual void PlayFunction() override;
    //virtual void StopFunction() override;

    friend class UIManager;
    friend class UIPanel;
};

