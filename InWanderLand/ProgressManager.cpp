#include "ProgressManager.h"

#include "ProgressTracker.h"

namespace application
{
	void ProgressManager::NotifyInitializeAll()
	{
		for (auto& each : trackerList)
		{
			each->ProgressInitialize();
		}
	}

	void ProgressManager::NotifyCurrentProgressSaveAll()
	{
		for (auto& each : trackerList)
		{
			each->CurrentProgressSave();
		}
	}

	void ProgressManager::NotifyRecoveryAll()
	{
		for (auto& each : trackerList)
		{
			each->Recovery();
		}
	}

	void ProgressManager::OnGameStart()
	{
		NotifyInitializeAll();
	}
}
