#include "UIPanel.h"
#include "UIButton.h"
#include "UIManager.h"

void UIPanel::SetWindowImage(yunutyEngine::graphics::UIImage* img)
{
	m_windowImage = img;
	m_panelObjects.push_back(m_windowImage->GetGameObject());
}

void UIPanel::SetUIButtonComponent(UIButton* btn)
{
	m_panelObjects.push_back(btn->GetGameObject());
	btn->SetLayer(m_windowImage->GetGI().GetLayer() + 1);;
}

void UIPanel::SetPanelActive(bool p_boolen)
{
	for (auto e : m_panelObjects)
	{
		e->SetSelfActive(p_boolen);
	}
	if (m_closeImageObject != nullptr)
		m_closeImageObject->SetSelfActive(p_boolen);

	isPanelActivated = p_boolen;
}

void UIPanel::SetCloseButtonActive(bool p_boolen)
{
	if (p_boolen)
	{
		m_closeImageObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		closeImageComponent = m_closeImageObject->AddComponent<yunutyEngine::graphics::UIImage>();
		closeButtonComponent = m_closeImageObject->AddComponent<UIButton>();
		m_closeButtonImage = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/Ingame/_0050_Button_Close.png");
		closeButtonComponent->SetImageComponent(closeImageComponent);
		//closeButtonComponent->SetIdleImage(m_closeButtonImage);
		closeButtonComponent->SetLayer(m_windowImage->GetGI().GetLayer() + 2);
		closeButtonComponent->m_mouseLiftedEventFunction = [=]()
		{
			for (auto e : m_panelObjects)
			{
				e->SetSelfActive(false);
			}
			closeButtonComponent->GetGameObject()->SetSelfActive(false);
			UIManager::Instance().ReportMouseExitButton(closeButtonComponent);
			this->isPanelActivated = false;
			if (m_parentPanel != nullptr)
			{
				m_parentPanel->SetPanelActive(true);
			}
		};
		m_closeImageObject->GetTransform()->SetWorldPosition({ m_windowImage->GetGameObject()->GetTransform()->GetWorldPosition().x + m_windowImage->GetGI().GetWidth() - m_closeButtonImage->GetWidth(),
			m_windowImage->GetGameObject()->GetTransform()->GetWorldPosition().y, 0 });
	}
}

void UIPanel::SetParentPanel(UIPanel* p_parentPanel)
{
	m_parentPanel = p_parentPanel;
}

void UIPanel::PlayFunction()
{

}

void UIPanel::StopFunction()
{
	yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
}

bool UIPanel::GetPanelActive() const
{
	return isPanelActivated;
}

void UIPanel::Start()
{
	if (m_closeImageObject != nullptr)
		m_closeImageObject->SetSelfActive(false);
	
}

