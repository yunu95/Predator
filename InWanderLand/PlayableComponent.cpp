#include "PlayableComponent.h"

namespace application
{
	std::unordered_set<PlayableComponent*> PlayableComponent::list = std::unordered_set<PlayableComponent*>();

	PlayableComponent::PlayableComponent()
	{
		list.insert(this);
	}

	PlayableComponent::~PlayableComponent()
	{
		list.erase(this);
	}

	void PlayableComponent::PreMapLoadAll()
	{
		for (auto& each : list)
		{
			each->PreMapLoad();
		}
	}

	void PlayableComponent::PostMapLoadAll()
	{
		for (auto& each : list)
		{
			each->PostMapLoad();
		}
	}

	void PlayableComponent::OnGameStartAll()
	{
		for (auto& each : list)
		{
			each->OnGameStart();
		}
	}

	void PlayableComponent::OnGamePauseAll()
	{
		for (auto& each : list)
		{
			each->OnGamePause();
		}
	}

	void PlayableComponent::OnGameResumeAll()
	{
		for (auto& each : list)
		{
			each->OnGameResume();
		}
	}

	void PlayableComponent::OnGameStopAll()
	{
		for (auto& each : list)
		{
			each->OnGameStop();
		}
	}
}
