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

void UIButton::SetButtonClickFunction(std::function<void()> p_func)
{
	m_ClickedEventFunction = p_func;
}

void UIButton::SetCloseButton()
{
	m_closeImageObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	m_closeImageComponent = m_closeImageObject->AddComponent<yunutyEngine::graphics::UIImage>();
	m_closeImageComponent->GetGI().SetLayer(m_layer + 1);
	m_closeButtonImage = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/UI/InGameUITemp/CloseButton.jpg");
	m_closeImageComponent->GetGI().SetImage(m_closeButtonImage);
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

void UIButton::Start()
{
	assert(m_IdleImage != nullptr);

	m_ImageComponent->GetGI().SetImage(m_IdleImage);
	m_CurrentImage = m_IdleImage;

	/// Width와 Height은 변경되지 않는다는 것을 전제로...
	m_Width = m_IdleImage->GetWidth();
	m_Height = m_IdleImage->GetHeight();

	Vector2d leftTopPos = GetTransform()->GetWorldPosition();
	m_ImageCenterPostion = Vector2d(leftTopPos.x + m_Width / 2, leftTopPos.y + m_Height / 2);

	if (m_closeImageObject != nullptr)
	{
		m_closeImageObject->GetTransform()->SetWorldPosition({ GetTransform()->GetWorldPosition().x + m_Width - m_closeButtonImage->GetWidth(), GetTransform()->GetWorldPosition().y, 0 });
		m_closeButtonCenterPosition = Vector2d(GetTransform()->GetWorldPosition().x + m_Width - (m_closeButtonImage->GetWidth() / 2), GetTransform()->GetWorldPosition().y + (m_closeButtonImage->GetHeight() / 2));
	}

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
	bool isMouseOnCloseButton = false;
	static bool isMouseUIMode = false;

	if (m_closeImageObject != nullptr &&
		mousePos.x <= m_closeButtonCenterPosition.x + m_Width / 2 && mousePos.x >= m_closeButtonCenterPosition.x - m_Width / 2 &&
		mousePos.y <= m_closeButtonCenterPosition.y + m_Height / 2 && mousePos.y >= m_closeButtonCenterPosition.y - m_Height / 2)
	{
		isMouseOnCloseButton = true;
		if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::MouseLeftClick))
		{
			m_closeImageObject->SetSelfActive(false);
			GetGameObject()->SetSelfActive(false);
		}
	}


	if (!isMouseOnCloseButton)
	{
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

}
