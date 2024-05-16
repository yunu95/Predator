#include "UIButton.h"
#include "UIManager.h"
#include "RTSCam.h"
#include "ContentsLayer.h"
#include "Application.h"

void UIButton::SetIdleImage(yunuGI::ITexture* p_IdleImage)
{
	//m_IdleImage = p_IdleImage;
	//m_ImageComponent->GetGI().SetImage(m_IdleImage);
}

void UIButton::SetClickedImage(yunuGI::ITexture* p_ClickedImage)
{
	m_ClickedImage = p_ClickedImage;
}

void UIButton::SetButtonClickFunction(std::function<void()> p_func)
{
	m_mouseLiftedEventFunction = p_func;
}
void UIButton::AddButtonClickFunction(std::function<void()> p_func)
{
	m_mouseLiftedEventFunctions.push_back(p_func);
}
void UIButton::AddButtonOnMouseFunction(std::function<void()> p_func)
{
	m_OnMouseEventFunctions.push_back(p_func);
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
	OnTransformUpdate();

	//m_ImageComponent->GetGI().SetImage(m_IdleImage);
	//m_CurrentImage = m_IdleImage;


	m_onMouseFunction = [=]()
		{
			for (auto& each : m_OnMouseEventFunctions)
			{
				each();
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
			if (m_mouseLiftedEventFunction != nullptr)
			{
				m_mouseLiftedEventFunction();
			}
			if (!m_mouseLiftedEventFunctions.empty())
			{
				for (auto& each : m_mouseLiftedEventFunctions)
				{
					each();
				}
			}
		};
}
void UIButton::OnTransformUpdate()
{
	/// Width와 Height은 변경되지 않는다는 것을 전제로...
	m_Width = m_ImageComponent->GetGI().GetWidth();
	m_Height = m_ImageComponent->GetGI().GetHeight();

	Vector2d leftTopPos = GetTransform()->GetWorldPosition();
	leftTopPos -= Vector2d::right * m_Width * m_ImageComponent->GetGI().GetXPivot();
	leftTopPos -= Vector2d::up * m_Height * m_ImageComponent->GetGI().GetYPivot();

	auto resolution = graphics::Renderer::SingleInstance().GetResolution();
	HWND hWnd = GetForegroundWindow();
	RECT wndRect;
	GetClientRect(hWnd, &wndRect);

	initialRectRight = wndRect.right;
	initialRectBottom = wndRect.bottom;
	Vector3d fixedLeftTopPos = Vector3d(leftTopPos.x / 1920 * resolution.x, leftTopPos.y / 1080 * resolution.y, 0);
	//GetTransform()->SetWorldPosition(fixedLeftTopPos);
	m_ImageCenterPostion = Vector2d(fixedLeftTopPos.x + m_Width / 2, fixedLeftTopPos.y + m_Height / 2);
}

void UIButton::Update()
{
	auto mousePos = Input::getMouseScreenPositionNormalized();
	mousePos.x *= 1920;
	mousePos.y *= 1080;
	bool isMouseInButton = false;

	if (mousePos.x <= m_ImageCenterPostion.x + (m_Width / 2) && mousePos.x >= m_ImageCenterPostion.x - (m_Width / 2) &&
		mousePos.y <= m_ImageCenterPostion.y + (m_Height / 2) && mousePos.y >= m_ImageCenterPostion.y - (m_Height / 2))
	{
		isMouseInButton = true;
	}

	//UIManager::Instance().ReportButtonOnMouse(this);
	if (isMouseInButton)
	{
		UIManager::Instance().ReportButtonOnMouse(this);
	}
	else
	{
		UIManager::Instance().ReportMouseExitButton(this);
	}
	//if (isMouseInButton && !isMouseNowOnButton)
	//{
	//    isMouseNowOnButton = true;
	//    isMouseInButton = false;

	//    UIManager::Instance().ReportButtonOnMouse(this);
	//    //isButtonOnMouseState = false;
	//}
	//else if (!isMouseInButton && isMouseNowOnButton)
	//{
	//    isMouseNowOnButton = false;
	//    UIManager::Instance().ReportMouseExitButton(this);
	//    //m_ImageComponent->GetGI().SetImage(m_IdleImage);
	//}
}
void UIButton::OnEnable()
{
	Update();
}
void UIButton::OnDisable()
{
	isMouseNowOnButton = false;
	UIManager::Instance().ReportMouseExitButton(this);
}
void UIButton::InvokeButtonClickEvent()
{
	if (m_mouseLiftedFunction)
	{
		m_mouseLiftedFunction();
	}
	if (!m_mouseLiftedEventFunctions.empty())
	{
		for (auto& each : m_mouseLiftedEventFunctions)
		{
			each();
		}
	}
}

//void UIButton::PlayFunction()
//{
//
//}
//
//void UIButton::StopFunction()
//{
//    yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
//}
