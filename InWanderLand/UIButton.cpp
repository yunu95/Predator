#include "UIButton.h"
#include "UIManager.h"
#include "RTSCam.h"

void UIButton::SetIdleImage(yunuGI::ITexture* p_IdleImage)
{
	m_IdleImage = p_IdleImage;
}

void UIButton::SetOnMouseImage(yunuGI::ITexture* p_OnMouseImage)
{
	m_MouseOnImage = p_OnMouseImage;
}

void UIButton::SetClickedImage(yunuGI::ITexture* p_ClickedImage)
{
	m_ClickedImage = p_ClickedImage;
}

void UIButton::SetLayer(int p_layerNum)
{
	m_layer = p_layerNum;
	m_ImageComponent->GetGI().SetLayer(p_layerNum);
}

int UIButton::GetLayer() const
{
	return m_layer;
}

void UIButton::SetImageComponent(yunutyEngine::graphics::UIImage* p_ImageComponent)
{
	m_ImageComponent = p_ImageComponent;
}

void UIButton::SetWidth(double p_Width)
{
	m_Width = p_Width;
}

void UIButton::SetHeight(double p_Height)
{
	m_Height = p_Height;
}

void UIButton::Start()
{
	assert(m_IdleImage != nullptr);

	m_ImageComponent->GetGI().SetImage(m_IdleImage);
	m_CurrentImage = m_IdleImage;

	Vector2d leftTopPos = GetTransform()->GetWorldPosition();
	m_ImageCenterPostion = Vector2d(leftTopPos.x + m_Width / 2, leftTopPos.y + m_Height / 2);

	m_ClickedEventFunction = [=]()
	{
		//m_ImageComponent->GetGI().SetImage(m_ClickedImage);
		auto sound = yunutyEngine::SoundSystem::PlayMusic("..\Bin\x64\Debug\Texture\BGM.mp3");
	};

	m_OnMouseEventFunction = [=]()
	{
		//m_ImageComponent->GetGI().SetImage(m_MouseOnImage);
	};

}

void UIButton::Update()
{
	auto mousePos = Input::getMouseScreenPosition();
	bool isMouseOnThisButton = false;

	static bool isMouseUIMode = false;

	if (mousePos.x <= m_ImageCenterPostion.x + m_Width / 2 && mousePos.x >= m_ImageCenterPostion.x - m_Width / 2 &&
		mousePos.y <= m_ImageCenterPostion.y + m_Height / 2 && mousePos.y >= m_ImageCenterPostion.y - m_Height / 2)
	{
		isMouseOnThisButton = true;
		isMouseUIMode = true;
		isMouseWasOnButton = true;
	}

	if (isMouseOnThisButton && m_CurrentImage != m_MouseOnImage)
	{
		UIManager::SingleInstance().ReportButtonOnMouse(this);

		m_CurrentImage = m_MouseOnImage;
	}
	else if (!isMouseOnThisButton && m_CurrentImage != m_IdleImage)
	{
		if (isMouseWasOnButton)
		{
			UIManager::SingleInstance().ReportMouseExitButton(this);
			isMouseWasOnButton = false;
		}

		m_ImageComponent->GetGI().SetImage(m_IdleImage);
		m_CurrentImage = m_IdleImage;
	}
}
