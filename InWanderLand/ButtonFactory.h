#pragma once
#include "YunutyEngine.h"
/// <summary>
/// Button을 main문에서 손쉽게 만들 수 있도록 해주는 클래스.
/// </summary>
class ButtonFactory : public SingletonClass<ButtonFactory>
{
private:
	yunutyEngine::graphics::UIImage* m_buttonUIImageComponent;
	yunuGI::ITexture* m_idleTexture;
	yunuGI::ITexture* m_OnMouseTexture;
	yunuGI::ITexture* m_ClickedTexture;

public:
	void CreateButton();
};

