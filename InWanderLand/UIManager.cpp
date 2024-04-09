#include "UIManager.h"
#include "RTSCam.h"
#include "UIManager.h"

void UIManager::LoadUITextures() const
{
	auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	rsrcMgr->LoadFile("Texture/Ingame/_0000_Icon_Menu.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0001_Frame_Tactics.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0002_Frame_MpBar.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0003_Bar_MpBar.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0004_BackFrame_MpBar.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0005_Frame_HpBar.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0006_Bar_HpBar.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0007_BackFrame_HpBar.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0008_Icon_Heal.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0009_Icon_Buff.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0010_Icon_Poison.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0011_Icon_Bleeding.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0012_Skill_RobinE.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0013_Skill_UrsulaE.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0014_Skill_UrsulaQ.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0015_Skill_HanselE.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0016_Skill_HanselQ.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0017_Skill_RobinQ.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0018_Portrait_Hansel.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0019_Portrait_Ursula.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0020_Portrait_Robin.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0021_Button_SkillUpgrade_Act.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0022_Button_SkillUpgrade_UnAct.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0023_Quest2_UnCompleted.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0024_Quest2_Complete.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0025_Quest3_Complete.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0026_Window_Quest.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0027_Quest1_Complete.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0028_Quest3_UnComplete.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0029_Quest1_UnComplete.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0030_Font_0.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0031_Font_9.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0032_Font_8.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0033_Font_7.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0034_Font_6.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0035_Font_5.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0036_Font_4.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0037_Font_3.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0038_Font_2.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0039_Font_1.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0040_System_ComboRate.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0041_Icon_SkillUnlocked.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0042_Icon_SkillLocked.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0043_RollOver_RobinSkill1_0.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0044_Button_No.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0045_Button_Yes.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0046_PopUp_UpgradeCheck.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0047_PopUp_Warning_SPT2.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0048_PopUp_Warning_SPT1.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0049_Window_SkillUpgrade.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0050_Button_Close.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0051_Toggle_On.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0052_Toggle_Off.png");
	rsrcMgr->LoadFile("Texture/Ingame/_0053_Window_AudioSetting.png");
	rsrcMgr->LoadFile("Texture/Ingame_Menu/Button_HowToPlay.png");
	rsrcMgr->LoadFile("Texture/Ingame_Menu/Button_Restart.png");
	rsrcMgr->LoadFile("Texture/Ingame_Menu/Button_Sound.png");
	rsrcMgr->LoadFile("Texture/Ingame_Menu/PopUp_Guide.png");
	rsrcMgr->LoadFile("Texture/Ingame_Menu/PopUp_IngameMenu.png");
}

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

bool UIManager::IsMouseOnButton()
{
	return isButtonActiviated;
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
