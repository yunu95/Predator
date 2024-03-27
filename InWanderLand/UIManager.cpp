#include "UIManager.h"
#include "RTSCam.h"
#include "UIManager.h"

void UIManager::ReportButtonOnMouse(UIButton* p_btn)
{
	// 2. Layer에 따라 현재 상호작용할 버튼을 정합니다.
	if (m_currentHighestLayer < p_btn->GetLayer())
	{
		if (m_highestPriorityButton != nullptr)				// 이전에 선택됐던 버튼의 이미지를 Idle로 초기화
			m_highestPriorityButton->m_ImageComponent->GetGI().SetImage(m_highestPriorityButton->m_IdleImage);

		/// highestPriorityButton 재정의
		m_currentHighestLayer = p_btn->GetLayer();
		m_highestPriorityButton = p_btn;
	}

	// 3. 멤버 리스트에 버튼을 담아줍니다.
	m_currentSelectedButtonList.push_back(p_btn);

	/// 4. 현재 상호작용할 버튼의 click함수로 leftClick함수를 재정의합니다.
	// 마우스를 올렸을 때 호출되는 함수도 호출합니다.
	if (m_highestPriorityButton != nullptr && m_highestPriorityButton->m_onMouseFunction != nullptr)
		m_highestPriorityButton->m_onMouseFunction();

	/// 버튼 클릭이 가능하도록 bool값을 켜줍니다.
	isButtonActiviated = true;
}

void UIManager::ReportMouseExitButton(UIButton* p_btn)
{
	p_btn->m_ImageComponent->GetGI().SetImage(p_btn->m_IdleImage);
	m_currentSelectedButtonList.remove(p_btn);

	/// 벗어난 후 마우스가 ui 위에 존재하지 않는다면...
	if (m_currentSelectedButtonList.empty())
	{
		m_highestPriorityButton = nullptr;
		m_currentHighestLayer = 0;
		isButtonActiviated = false;
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

			m_highestPriorityButton->m_onMouseFunction();

			isButtonActiviated = true;
		}
	}
}

void UIManager::Update()
{
	if (m_currentSelectedButtonList.empty())
	{
		InputManager::Instance().IsMouseOnUI(false);
	}
	else
	{
		InputManager::Instance().IsMouseOnUI(true);
	}

	if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::MouseLeftClick))
	{
		if (isButtonActiviated)
		{
			m_highestPriorityButton->m_mousePushedFunction();
		}
	}

	if (yunutyEngine::Input::isKeyLifted(yunutyEngine::KeyCode::MouseLeftClick))
	{
		if (m_highestPriorityButton != nullptr)
		{
			m_highestPriorityButton->m_mouseLiftedFunction();
		}
	}
}
