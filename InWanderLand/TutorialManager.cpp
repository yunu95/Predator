#include "TutorialManager.h"


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
        SetTutorialMode(false);
	}

	void TutorialManager::OnGameStart()
	{
        SetTutorialMode(false);
	}

	void TutorialManager::OnGameStop()
	{
        SetTutorialMode(false);
	}

	void TutorialManager::SetTutorialMode(bool mode)
	{
        if (!isTutorialMode && mode)
        {
            isTutorialMode = mode;        
		}
        else if (isTutorialMode && !mode)
        {
            isTutorialMode = mode;
		}
	}
}