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
}

void UIPanel::SetPanelActive(bool p_boolen)
{
	for (auto e : m_panelObjects)
	{
		e->SetSelfActive(p_boolen);
	}
	if (m_closeImageObject != nullptr)
		m_closeImageObject->SetSelfActive(p_boolen);
}

void UIPanel::SetCloseButtonActive(bool p_boolen)
{
	if (p_boolen)
	{
		m_closeImageObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		closeImageComponent = m_closeImageObject->AddComponent<yunutyEngine::graphics::UIImage>();
		closeButtonComponent = m_closeImageObject->AddComponent<UIButton>();
		m_closeButtonImage = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/UI/InGameUITemp/CloseButton.jpg");
		closeButtonComponent->SetImageComponent(closeImageComponent);
		closeButtonComponent->SetIdleImage(m_closeButtonImage);
		//closeButtonComponent->SetLayer(m_windowImage->GetGI().GetLayer() + 1);
		closeButtonComponent->SetLayer(1000);
		closeButtonComponent->m_mouseLiftedEventFunction = [=]()
		{
			for (auto e : m_panelObjects)
			{
				e->SetSelfActive(false);
			}
			closeButtonComponent->GetGameObject()->SetSelfActive(false);
			UIManager::SingleInstance().ReportMouseExitButton(closeButtonComponent);
		};
		m_closeImageObject->GetTransform()->SetWorldPosition({ m_windowImage->GetGameObject()->GetTransform()->GetWorldPosition().x + m_windowImage->GetGI().GetWidth() - m_closeButtonImage->GetWidth(),
			m_windowImage->GetGameObject()->GetTransform()->GetWorldPosition().y, 0 });
	}
}

void UIPanel::Start()
{
	if (m_closeImageObject != nullptr)
		m_closeImageObject->SetSelfActive(false);
	
}

