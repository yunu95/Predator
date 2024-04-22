#pragma once
#include "YunutyEngine.h"
#include "JsonUIData.h"
#include "UIExportFlag.h"
#include "UIOffsetTransition.h"
#include "PopupOnEnable.h"
#include "UIImage.h"

class UIButton;
class FloatFollower;
// 임포트된 UI 요소에 대한 정보를 잔뜩 저장하는 클래스
class UIElement : public Component
{
public:
	virtual void Start() override;
	JsonUIData importedUIData;
	PopupOnEnable* scalePopUpTransition{ nullptr };
	UIOffsetTransition* enableTransition{ nullptr };
	UIOffsetTransition* disableTransition{ nullptr };
	graphics::UIImage* imageComponent{ nullptr };
	graphics::UIText* textComponent{ nullptr };
	UIButton* button{ nullptr };
	void EnableElement()
	{
		GetGameObject()->SetSelfActive(true);
		if (scalePopUpTransition != nullptr)
		{
			scalePopUpTransition->ActivateTimer();
		}
		if (enableTransition != nullptr)
		{
			enableTransition->ActivateTimer();
		}
	}
	void DisableElement()
	{
		bool disablingHandled = false;
		if (disableTransition != nullptr)
		{
			disablingHandled = true;
			disableTransition->ActivateTimer();
		}
		if (disablingHandled == false)
		{
			GetGameObject()->SetSelfActive(false);
		}
	}
	// UI 요소에 영향을 줄 수 있는 실수 값을 조정합니다.
	FloatFollower* adjuster{ nullptr };
	friend class UIManager;
};
