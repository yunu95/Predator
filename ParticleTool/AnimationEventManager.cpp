#include "AnimationEventManager.h"

#include "YunutyEngine.h"
#include "ParticleTool_Manager.h"

namespace application
{
	void AnimationEventManager::Init()
	{
		auto& pm = particle::ParticleTool_Manager::GetSingletonInstance();

		skinnedObjList = &pm.skinnedObjList;
		particleInstanceList = &pm.particleInstanceList;
		particleInstanceIDMap = &pm.particleInstanceIDMap;
		aniMap = &pm.aniMap;
		aniNameMap = &pm.aniNameMap;
	}

	void AnimationEventManager::Clear()
	{
		for (auto& eachEvent : eventList)
		{
			auto obj = (*skinnedObjList)[eachEvent->fbxName];
			auto animator = obj->GetComponent<graphics::Animator>();
			int index = 0;
			for (auto& each : (*aniNameMap)[eachEvent->fbxName])
			{
				if (each == eachEvent->animationName)
				{
					break;
				}
				index++;
			}
			auto ani = (*aniMap)[eachEvent->fbxName][index];

			if (!animator || !ani)
			{
				continue;
			}

			animator->EraseAnimationFunc(ani, eventFuncIndexList[eachEvent]);
		}
		eventList.clear();
		eventFuncIndexList.clear();
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
		auto obj = (*skinnedObjList)[event->fbxName];
		auto animator = obj->GetComponent<graphics::Animator>();
		int index = 0;
		for (auto& each : (*aniNameMap)[event->fbxName])
		{
			if (each == event->animationName)
			{
				break;
			}
			index++;
		}
		auto ani = (*aniMap)[event->fbxName][index];
		
		if (!animator || !ani)
		{
			return false;
		}

		auto type = event->GetType();
		unsigned long long funcIndex = 0;
		switch (type)
		{
			case application::AnimationEventType::GameObject_ActivateEvent:
			{
				auto ptr = static_cast<GameObject_ActivateEvent*>(event.get());
				GameObject* particle = nullptr;
				for (auto& each : (*particleInstanceList)[event->fbxName])
				{
					if (each->name == ptr->objName)
					{
						particle = (*particleInstanceIDMap)[each];
						break;
					}
				}

				funcIndex = animator->PushAnimationWithFunc(ani, event->frame,[=]()
					{
						particle->SetSelfActive(true);
						auto ptr = particle->GetComponent<graphics::ParticleRenderer>();
						ptr->Play();
					});

				if (funcIndex == 0)
				{
					return false;
				}
				else
				{
					eventFuncIndexList[event] = funcIndex;
				}

				break;
			}
			case application::AnimationEventType::GameObject_DisabledEvent:
			{
				auto ptr = static_cast<GameObject_DisabledEvent*>(event.get());
				GameObject* particle = nullptr;
				for (auto& each : (*particleInstanceList)[event->fbxName])
				{
					if (each->name == ptr->objName)
					{
						particle = (*particleInstanceIDMap)[each];
						break;
					}
				}

				funcIndex = animator->PushAnimationWithFunc(ani, event->frame, [=]()
					{
						particle->SetSelfActive(false);
					});

				if (funcIndex == 0)
				{
					return false;
				}
				else
				{
					eventFuncIndexList[event] = funcIndex;
				}

				break;
			}
			case application::AnimationEventType::Sound_PlayOnceEvent:
			{
				auto ptr = static_cast<Sound_PlayOnceEvent*>(event.get());
				break;
			}
			case application::AnimationEventType::Sound_PlayLoopEvent:
			{
				auto ptr = static_cast<Sound_PlayLoopEvent*>(event.get());
				break;
			}
			default:
				break;
		}

		eventList.insert(event);
		
		return true;
	}

	bool AnimationEventManager::EraseAnimationEvent(const std::shared_ptr<AnimationEvent>& event)
	{
		if (!eventFuncIndexList.contains(event))
		{
			return false;
		}

		auto obj = (*skinnedObjList)[event->fbxName];
		auto animator = obj->GetComponent<graphics::Animator>();
		int index = 0;
		for (auto& each : (*aniNameMap)[event->fbxName])
		{
			if (each == event->animationName)
			{
				break;
			}
			index++;
		}
		auto ani = (*aniMap)[event->fbxName][index];

		if (!animator || !ani)
		{
			return false;
		}

		animator->EraseAnimationFunc(ani, eventFuncIndexList[event]);

		eventList.erase(event);
		eventFuncIndexList.erase(event);
		return true;
	}
}
