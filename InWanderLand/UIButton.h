#pragma once
#include "YunutyEngine.h"
#include "InputManager.h"

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

	double m_Width;
	double m_Height;

	Vector2d m_ImageCenterPostion;

	bool isMouseWasOnButton;

	int m_layer;

public:
	void SetIdleImage(yunuGI::ITexture* p_IdleImage);
	void SetOnMouseImage(yunuGI::ITexture* p_OnMouseImage);
	void SetClickedImage(yunuGI::ITexture* p_ClickedImage);

	void SetLayer(int p_layerNum);
	int GetLayer() const;

	void SetImageComponent(yunutyEngine::graphics::UIImage* p_ImageComponent);

	void SetWidth(double p_Width);
	void SetHeight(double p_Height);

	virtual void Start() override;
	virtual void Update() override;

	friend class UIManager;
};

