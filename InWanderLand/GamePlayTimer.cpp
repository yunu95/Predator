#include "GamePlayTimer.h"

#include "Application.h"

namespace application
{
	void GamePlayTimer::SingletonInitializer()
	{

	}

	void GamePlayTimer::Update()
	{
		if (shouldCheck)
		{
			playTime += Time::GetDeltaTime();
			playRealTime += Time::GetDeltaTimeUnscaled();

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
			repeatTimeChecker[time] = time;
		}

		for (auto& [time, func] : repeatCallbackReal)
		{
			repeatTimeCheckerReal[time] = time;
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
		if (Application::GetInstance().IsContentsPlaying())
		{
			shouldCheck = false;
		}
	}

	void GamePlayTimer::ResumeTimer()
	{
		if (Application::GetInstance().IsContentsPlaying())
		{
			shouldCheck = true;
		}
	}
}
