#include "AnimationEventManager.h"

#include "YunutyEngine.h"


namespace application
{
	void AnimationEventManager::Clear()
	{
		for (auto& each : eventList)
		{
			auto type = each->GetType();
			switch (type)
			{
				case application::AnimationEventType::GameObject_ActivateEvent:
				{
					auto ptr = static_cast<GameObject_ActivateEvent*>(each.get());
					break;
				}
				case application::AnimationEventType::GameObject_DisabledEvent:
				{
					auto ptr = static_cast<GameObject_DisabledEvent*>(each.get());
					break;
				}
				case application::AnimationEventType::Sound_PlayOnceEvent:
				{
					auto ptr = static_cast<Sound_PlayOnceEvent*>(each.get());
					break;
				}
				case application::AnimationEventType::Sound_PlayLoopEvent:
				{
					auto ptr = static_cast<Sound_PlayLoopEvent*>(each.get());
					break;
				}
				default:
					break;
			}
		}
		eventList.clear();
	}


	bool AnimationEventManager::Load(const json& data)
	{
		return true;
	}

	bool AnimationEventManager::Save(json& data)
	{
		return true;
	}

	bool AnimationEventManager::AddAnimationEvent(const std::shared_ptr<AnimationEvent>& event)
	{
		return false;
	}

	bool AnimationEventManager::EraseAnimationEvent(const std::shared_ptr<AnimationEvent>& event)
	{
		return false;
	}
}
