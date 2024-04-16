#include "CinematicManager.h"

#include "GamePlayTimer.h"

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
        isCinematicMode = false;
    }

    void CinematicManager::OnGameStart()
    {
        isCinematicMode = false;
    }

    void CinematicManager::OnGameStop()
    {
        isCinematicMode = false;
    }

    void CinematicManager::SetCinematicMode(bool isCinematicMode)
    {
        if (!this->isCinematicMode && isCinematicMode)
        {
            this->isCinematicMode = isCinematicMode;
            GamePlayTimer::Instance().PauseTimer();
            // UIManager::Hide();
            // InputManager::T();

            std::cout << "\n======================\n";
            std::cout << "  CinematicMode [On]";
            std::cout << "\n======================\n";
        }
        else if (this->isCinematicMode && !isCinematicMode)
        {
            this->isCinematicMode = isCinematicMode;
            GamePlayTimer::Instance().ResumeTimer();
            // UIManager::Show();
            // InputManager::T();

            std::cout << "\n======================\n";
            std::cout << "  CinematicMode [Off]";
            std::cout << "\n======================\n";
        }
    }
}
