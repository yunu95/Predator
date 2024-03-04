#pragma once
#include "YunutyEngine.h"
#include "InputManager.h"

/// <summary>
/// UI Button 클래스.
/// 클릭 시 호출되는 함수(m_ClickedEventFunction)를 정의할 경우 버튼이 됩니다.
/// SetCloseButton() 함수로 닫는 창을 활성화 시킬 수 있습니다.
/// </summary>
class UIButton : public Component
{
private:
	yunuGI::ITexture* m_IdleImage;
	yunuGI::ITexture* m_MouseOnImage;
	yunuGI::ITexture* m_ClickedImage;

	yunuGI::ITexture* m_CurrentImage;

	std::function<void()> m_ClickedEventFunction;
	std::function<void()> m_OnMouseEventFunction;

	yunutyEngine::graphics::UIImage* m_ImageComponent;

	GameObject* m_closeImageObject;
	yunutyEngine::graphics::UIImage* m_closeImageComponent;
	yunuGI::ITexture* m_closeButtonImage;

	double m_Width;
	double m_Height;

	Vector2d m_ImageCenterPostion;
	Vector2d m_closeButtonCenterPosition;

	bool isMouseWasOnButton;
	int m_layer;

public:
	void SetIdleImage(yunuGI::ITexture* p_IdleImage);
	void SetOnMouseImage(yunuGI::ITexture* p_OnMouseImage);
	void SetClickedImage(yunuGI::ITexture* p_ClickedImage);

	void SetButtonClickFunction(std::function<void()> p_func);

	void SetCloseButton();

	void SetLayer(int p_layerNum);
	int GetLayer() const;

	void SetImageComponent(yunutyEngine::graphics::UIImage* p_ImageComponent);

	virtual void Start() override;
	virtual void Update() override;

	friend class UIManager;
};

