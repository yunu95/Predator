#include "TutorialManager.h"

#include "GameManager.h"
#include "GamePlayTimer.h"
#include "UIManager.h"
#include "UIElement.h"
#include "InputManager.h"

#include <iostream>

namespace application
{
	void TutorialManager::SingletonInitializer()
	{

	}

	void TutorialManager::Update()
	{

	}

	void TutorialManager::PreMapLoad()
	{
		isTutorialMode = false;
        tutorialIndex = 0;
	}

	void TutorialManager::OnGameStart()
	{
		isTutorialMode = false;
        tutorialIndex = 0;
	}

	void TutorialManager::OnGameStop()
	{
		isTutorialMode = false;
        tutorialIndex = 0;
	}

	void TutorialManager::SetTutorialMode(bool mode)
	{
        if (!isTutorialMode && mode)
        {
            isTutorialMode = mode;
            AllStop();



            /// Debug
            std::cout << "\n======================\n";
            std::cout << "  TutorialMode [On]";
            std::cout << "\n======================\n";
        }
        else if (isTutorialMode && !mode)
        {
            isTutorialMode = mode;
            AllResume();



            /// Debug
            std::cout << "\n======================\n";
            std::cout << "  TutorialMode [Off]";
            std::cout << "\n======================\n";
        }
	}

    void TutorialManager::AllStop()
    {
        GamePlayTimer::Instance().PauseTimer();

        /// 전술모드 시스템을 응용하여 정지상태에 돌입합니다.
        /// 이 시점에서 PushTutorialTarget 을 통해 대상들을 지정하여 넘깁니다.
    }

    void TutorialManager::AllResume()
    {
        GamePlayTimer::Instance().ResumeTimer();

        if (isTutorialMode)
        {
            tutorialIndex++;
        }

        /// 전술모드 시스템에서 튜토리얼 스킬이 발동되었을 때, 해당 함수를 호출해주어야 합니다.
    }

    void TutorialManager::PushTutorialTarget(Unit::UnitType targetUnit, Unit::SkillEnum targetSkill)
    {
        /// 전술모드 시스템에 튜토리얼 세트를 등록하는 함수입니다.
    }
}