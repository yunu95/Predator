#include "UIButton.h"
#include "UIManager.h"
#include "RTSCam.h"
#include "ContentsLayer.h"
#include "Application.h"

void UIButton::SetIdleImage(yunuGI::ITexture* p_IdleImage)
{
    m_IdleImage = p_IdleImage;
    m_ImageComponent->GetGI().SetImage(m_IdleImage);
}

void UIButton::SetOnMouseImage(yunuGI::ITexture* p_OnMouseImage)
{
    m_MouseOnImage = p_OnMouseImage;
}

void UIButton::SetClickedImage(yunuGI::ITexture* p_ClickedImage)
{
    m_ClickedImage = p_ClickedImage;
}

void UIButton::SetButtonClickFunction(std::function<void()> p_func)
{
    m_mouseLiftedEventFunction = p_func;
}

void UIButton::SetLayer(int p_layerNum)
{
    //m_layer = p_layerNum;
    //m_ImageComponent->GetGI().SetLayer(p_layerNum);
}

int UIButton::GetLayer() const
{
    return m_ImageComponent->GetGI().GetLayer();
    //return m_layer;
}

void UIButton::SetImageComponent(yunutyEngine::graphics::UIImage* p_ImageComponent)
{
    m_ImageComponent = p_ImageComponent;
}

void UIButton::Start()
{
    assert(m_IdleImage != nullptr);

    m_ImageComponent->GetGI().SetImage(m_IdleImage);
    //m_CurrentImage = m_IdleImage;

    /// Width와 Height은 변경되지 않는다는 것을 전제로...
    m_Width = m_ImageComponent->GetGI().GetWidth();
    m_Height = m_ImageComponent->GetGI().GetHeight();

    Vector2d leftTopPos = GetTransform()->GetWorldPosition();

    HWND hWnd = GetForegroundWindow();
    RECT wndRect;
    GetClientRect(hWnd, &wndRect);

    initialRectRight = wndRect.right;
    initialRectBottom = wndRect.bottom;

    Vector3d fixedLeftTopPos = Vector3d(leftTopPos.x / 1920 * wndRect.right, leftTopPos.y / 1080 * wndRect.bottom, 0);
    GetTransform()->SetWorldPosition(fixedLeftTopPos);
    m_ImageCenterPostion = Vector2d(fixedLeftTopPos.x + m_Width / 2, fixedLeftTopPos.y + m_Height / 2);

    m_onMouseFunction = [=]()
        {
            if (m_MouseOnImage != nullptr)
            {
                m_ImageComponent->GetGI().SetImage(m_MouseOnImage);
            }
            if (m_OnMouseEventFunction != nullptr)
            {
                m_OnMouseEventFunction();
            }
        };

    m_mousePushedFunction = [=]()
        {
            if (m_ClickedImage != nullptr)
            {
                m_ImageComponent->GetGI().SetImage(m_ClickedImage);
            }
        };

    m_mouseLiftedFunction = [=]()
        {
            if (m_MouseOnImage != nullptr)
            {
                m_ImageComponent->GetGI().SetImage(m_MouseOnImage);
            }
            if (m_mouseLiftedEventFunction != nullptr)
            {
                m_mouseLiftedEventFunction();
            }
        };



}

void UIButton::Update()
{
    auto mousePos = Input::getMouseScreenPositionNormalized();
    mousePos.x *= 1920;
    mousePos.y *= 1080;
    bool isMouseJustEntered = false;

    if (mousePos.x <= m_ImageCenterPostion.x + (m_Width / 2) && mousePos.x >= m_ImageCenterPostion.x - (m_Width / 2) &&
        mousePos.y <= m_ImageCenterPostion.y + (m_Height / 2) && mousePos.y >= m_ImageCenterPostion.y - (m_Height / 2))
    {
        isMouseJustEntered = true;
    }

    if (isMouseJustEntered && !isMouseNowOnButton)
    {
        isMouseNowOnButton = true;
        isMouseJustEntered = false;

        UIManager::Instance().ReportButtonOnMouse(this);
        //isButtonOnMouseState = false;
    }
    else if (!isMouseJustEntered && isMouseNowOnButton)
    {
        isMouseNowOnButton = false;
        UIManager::Instance().ReportMouseExitButton(this);
        m_ImageComponent->GetGI().SetImage(m_IdleImage);
    }
}
void UIButton::OnDisable()
{
    UIManager::Instance().ReportMouseExitButton(this);
}
