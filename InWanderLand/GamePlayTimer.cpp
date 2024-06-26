#include "GamePlayTimer.h"

#include "Application.h"
#include "ContentsLayer.h"

namespace application
{
	void GamePlayTimer::SingletonInitializer()
	{

	}

	void GamePlayTimer::Update()
	{
		if (Application::GetInstance().IsContentsPlaying())
		{
			if (shouldCheck)
			{
				playTime += Time::GetDeltaTime();

				for (auto& [time, func] : repeatCallback)
				{
					if (playTime - repeatTimeChecker[time] >= time)
					{
						for (auto& each : func)
						{
							each();
						}
						repeatTimeChecker[time] = playTime;
					}
				}
			}

			playRealTime += Time::GetDeltaTimeUnscaled();

			for (auto& [time, func] : repeatCallbackReal)
			{
				if (playRealTime - repeatTimeCheckerReal[time] >= time)
				{
					for (auto& each : func)
					{
						each();
					}
					repeatTimeCheckerReal[time] = playRealTime;
				}
			}
		}
	}

	void GamePlayTimer::PreMapLoad()
	{
		repeatCallback.clear();
		repeatCallbackReal.clear();
		repeatTimeChecker.clear();
		repeatTimeCheckerReal.clear();
		shouldCheck = false;
		playTime = 0;
		playRealTime = 0;
	}

	void GamePlayTimer::OnGameStart()
	{
		shouldCheck = true;
		playTime = 0;
		playRealTime = 0;

		for (auto& [time, func] : repeatCallback)
		{
			repeatTimeChecker[time] = 0;
		}

		for (auto& [time, func] : repeatCallbackReal)
		{
			repeatTimeCheckerReal[time] = 0;
		}
	}

	void GamePlayTimer::OnGamePause()
	{
		shouldCheck = false;
	}

	void GamePlayTimer::OnGameResume()
	{
		shouldCheck = true;
	}

	void GamePlayTimer::OnGameStop()
	{
		repeatCallback.clear();
		repeatCallbackReal.clear();
		repeatTimeChecker.clear();
		repeatTimeCheckerReal.clear();
		shouldCheck = false;
		playTime = 0;
		playRealTime = 0;
	}

	void GamePlayTimer::PauseTimer()
	{
		shouldCheck = false;
	}

	void GamePlayTimer::ResumeTimer()
	{
		shouldCheck = true;
	}
}
