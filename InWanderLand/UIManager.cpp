#include "UIManager.h"
#include "RTSCam.h"
#include "UIManager.h"

void UIManager::ReportButtonOnMouse(UIButton* p_btn)
{
	// 1. 이전에 UI 외 다른 명령이 들어있었다면 멤버로 저장해둡니다.
	if (m_currentSelectedButtonList.empty())			
	{
		m_beforeUIEnterFunction = m_rtsCamComponent->groundLeftClickCallback;
	}

	// 2. Layer에 따라 현재 상호작용할 버튼을 정합니다.
	if (m_currentHighestLayer < p_btn->GetLayer())
	{
		if (m_highestPriorityButton != nullptr)
			m_highestPriorityButton->m_ImageComponent->GetGI().SetImage(m_highestPriorityButton->m_IdleImage);
		m_currentHighestLayer = p_btn->GetLayer();
		m_highestPriorityButton = p_btn;
	}

	// 3. 멤버 리스트에 버튼을 담아줍니다.
	m_currentSelectedButtonList.push_back(p_btn);

	/// 4. 현재 상호작용할 버튼의 click함수로 leftClick함수를 재정의합니다.
	// 마우스를 올렸을 때 호출되는 함수도 호출합니다.
	m_highestPriorityButton->m_ImageComponent->GetGI().SetImage(m_highestPriorityButton->m_MouseOnImage);
	m_highestPriorityButton->m_OnMouseEventFunction();
	m_rtsCamComponent->groundLeftClickCallback = [=](Vector3d pos)
	{
		m_highestPriorityButton->m_ImageComponent->GetGI().SetImage(m_highestPriorityButton->m_ClickedImage);
		m_highestPriorityButton->m_ClickedEventFunction();
	};
}

void UIManager::ReportMouseExitButton(UIButton* p_btn)
{
	p_btn->m_ImageComponent->GetGI().SetImage(p_btn->m_IdleImage);
	m_currentSelectedButtonList.remove(p_btn);

	if (m_currentSelectedButtonList.empty())
	{
		m_rtsCamComponent->groundLeftClickCallback = m_beforeUIEnterFunction;
	}
	/// 만약, 마우스가 벗어난 버튼이 Highest-Priority였다면 leftClick 함수를 재정의합니다.
	// 그러기 위해선, 다음 우선순위(layer)의 버튼을 재정의 해줘야합니다.
	else
	{
		if (p_btn == m_highestPriorityButton)
		{
			m_currentHighestLayer = 0;						// 초기화
			for (auto e : m_currentSelectedButtonList)
			{
				if (e->GetLayer() > m_currentHighestLayer)
				{
					m_currentHighestLayer = e->GetLayer();
					m_highestPriorityButton = e;
				}
			}

			m_highestPriorityButton->m_ImageComponent->GetGI().SetImage(m_highestPriorityButton->m_MouseOnImage);
			m_highestPriorityButton->m_OnMouseEventFunction();

			m_rtsCamComponent->groundLeftClickCallback = [=](Vector3d pos)
			{
				m_highestPriorityButton->m_ImageComponent->GetGI().SetImage(m_highestPriorityButton->m_ClickedImage);
				m_highestPriorityButton->m_ClickedEventFunction();
			};
		}
	}
}

void UIManager::SetRTSCam(RTSCam* p_RTSCamComponent)
{
	m_rtsCamComponent = p_RTSCamComponent;
}

void UIManager::Update()
{
	if (m_currentSelectedButtonList.empty())
	{
		InputManager::SingleInstance().IsMouseOnUI(false);
	}
	else
	{
		InputManager::SingleInstance().IsMouseOnUI(true);
	}
}
