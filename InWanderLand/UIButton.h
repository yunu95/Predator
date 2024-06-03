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
    std::vector<std::function<void()>> m_mouseLiftedEventFunctionsExternal;
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
    // 사용자가 직접 버튼을 클릭했을때 호출되는 함수, 여기로 등록된 함수들은 InvokeInternalButtonClickEvent()에서는 호출되지 않는다.
    void AddExternalButtonClickFunction(std::function<void()> p_func);
    void AddButtonOnMouseFunction(std::function<void()> p_func);

    void SetLayer(int p_layerNum);
    int GetLayer() const;

    void SetImageComponent(yunutyEngine::graphics::UIImage* p_ImageComponent);

    virtual void Start() override;
    virtual void OnTransformUpdate() override;
    virtual void Update() override;
    virtual void OnEnable() override;
    virtual void OnDisable() override;
    // 사용자가 직접 버튼을 눌렀을 때 호출되는 이벤트, 등록된 모든 이벤트들을 동작시킨다.
    virtual void InvokeButtonClickEvent();
    // UI가 다른 UI를 닫거나 여는 등 유니티에서 내부적으로 링킹된 이벤트들만 동작시킨다.
    virtual void InvokeInternalButtonClickEvent();

    //virtual void PlayFunction() override;
    //virtual void StopFunction() override;

    friend class UIManager;
    friend class UIPanel;
    bool clickable = true;
};

