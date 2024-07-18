#include "ProgressTracker.h"

#include "ProgressManager.h"

namespace application
{
	ProgressTracker::ProgressTracker()
	{
		ProgressManager::SingleInstance().trackerList.insert(this);
	}

	ProgressTracker::~ProgressTracker()
	{
		ProgressManager::SingleInstance().trackerList.erase(this);
	}

	void ProgressTracker::DoRecoveryCallbacks()
	{
		for (auto& [idx, each] : recoveryCallbacks)
		{
			each();
		}
	}

	unsigned long long ProgressTracker::AddRecoveryCallback(const std::function<void()>& func)
	{
		recoveryCallbacks[callbackIndex] = func;
		return callbackIndex++;
	}

	bool ProgressTracker::RemoveRecoveryCallback(unsigned long long funcIdx)
	{
		return recoveryCallbacks.erase(funcIdx);
	}
}
