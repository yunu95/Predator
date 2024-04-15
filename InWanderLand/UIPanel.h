#pragma once
#include "YunutyEngine.h"

/// <summary>
/// 하나의 창에 여러가지 버튼이 있을 경우 (ex. 메뉴 창)
/// 하나의 요소로써 작동하게 할 수 있도록 지원하는 컴포넌트.
/// 그냥 UIImage나 UIButton 컴포넌트를 갖고 있는 게임 오브젝트를 등록해주면 된다.
/// </summary>

// 사용하지 않음

class UIButton;

class UIPanel : public Component
{
private:
	std::vector<GameObject*> m_panelObjects;
	yunutyEngine::graphics::UIImage* m_windowImage;

	GameObject* m_closeImageObject;
	yunuGI::ITexture* m_closeButtonImage;
	yunutyEngine::graphics::UIImage* closeImageComponent;
	UIButton* closeButtonComponent;

	UIPanel* m_parentPanel;

	Vector2d m_closeButtonCenterPosition;

	bool isPanelActivated{ false };

public:
	void SetWindowImage(yunutyEngine::graphics::UIImage* img);
	void SetUIButtonComponent(UIButton* btn);
	void SetPanelActive(bool p_boolen);
	void SetCloseButtonActive(bool p_boolen);

	void SetParentPanel(UIPanel* p_parentPanel);

	bool GetPanelActive() const;
	virtual void Start() override;
};

