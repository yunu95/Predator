#pragma once
#include "YunutyEngine.h"
#include "UIButton.h"
/// <summary>
/// UIButton 객체들을 관리하는 컴포넌트.
/// 역할
/// 1. 모든 UIButton 객체들을 저장 및 관리
/// 2. button 멤버 UIImage의 Layer를 비교하여 클릭 함수 재정의
/// 3. 최소 하나의 button에 마우스가 올라가 있는가? 를 항상 판별.
/// </summary>

class RTSCam;

class UIManager : public SingletonClass<UIManager>
{
private:
	std::list<UIButton*> m_currentSelectedButtonList;

	RTSCam* m_rtsCamComponent;

	std::function<void(Vector3d pos)> m_beforeUIEnterFunction;

	int m_currentHighestLayer = 0;
	UIButton* m_highestPriorityButton;

	bool isButtonActiviated = false;

public:
	void ReportButtonOnMouse(UIButton* p_btn);
	void ReportMouseExitButton(UIButton* p_btn);

	void SetRTSCam(RTSCam* p_RTSCamComponent);
	//friend class UIButton;
	void Update();
};

