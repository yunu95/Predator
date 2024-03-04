#include "UIPanel.h"
#include "UIButton.h"

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
}

void UIPanel::SetCloseButtonActive(bool p_boolen)
{
	if (p_boolen)
	{
		auto closeImageObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto closeImageComponent = closeImageObject->AddComponent<yunutyEngine::graphics::UIImage>();
		//closeImageComponent->GetGI().SetLayer(m_windowImage->GetGI().GetLayer() + 1);
		auto closeButtonImage = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetTexture(L"Texture/UI/InGameUITemp/CloseButton.jpg");
		closeImageComponent->GetGI().SetImage(closeButtonImage);
	}
}
