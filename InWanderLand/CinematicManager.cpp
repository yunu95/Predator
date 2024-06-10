#include "CinematicManager.h"

#include "GamePlayTimer.h"
#include "UIManager.h"
#include "UIElement.h"
#include "InputManager.h"

#include <iostream>

namespace application
{
    void CinematicManager::SingletonInitializer()
    {

    }

    void CinematicManager::Update()
    {

    }

    void CinematicManager::PreMapLoad()
    {
        SetCinematicMode(false);
    }

    void CinematicManager::OnGameStart()
    {
        SetCinematicMode(false);
    }

    void CinematicManager::OnGameStop()
    {
        SetCinematicMode(false);
    }

    void CinematicManager::SetCinematicMode(bool isCinematicMode)
    {
        if (!this->isCinematicMode && isCinematicMode)
        {
            this->isCinematicMode = isCinematicMode;
            GamePlayTimer::Instance().PauseTimer();
            // InputManager::T();

            std::cout << "\n======================\n";
            std::cout << "  CinematicMode [On]";
            std::cout << "\n======================\n";

            // 인게임 UI 비활성화
            UIManager::Instance().SetIngameUIVisible(false);
            //GameManager::Instance().EngageCinematic();
        }
        else if (this->isCinematicMode && !isCinematicMode)
        {
            this->isCinematicMode = isCinematicMode;
            GamePlayTimer::Instance().ResumeTimer();
            // InputManager::T();

            std::cout << "\n======================\n";
            std::cout << "  CinematicMode [Off]";
            std::cout << "\n======================\n";

            // 인게임 UI 활성화
            UIManager::Instance().SetIngameUIVisible(true);
            //GameManager::Instance().EndCinematic();
        }
    }
}
