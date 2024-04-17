#include "CinematicManager.h"

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
            // UIManager::Hide();
            // InputManager::T();
            std::cout << "HideUI / LockInput!\n";
        }
        else if (this->isCinematicMode && !isCinematicMode)
        {
            this->isCinematicMode = isCinematicMode;
            // UIManager::Show();
            // InputManager::T();
            std::cout << "ShowUI / UnlockInput!\n";
        }
    }
}
