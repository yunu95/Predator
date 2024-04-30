#include "AnimationEventManager.h"

#include "YunutyEngine.h"
#include "ParticleTool_Manager.h"

extern bool ppisLoad;

void EraseSequenceData(const std::weak_ptr<application::AnimationEvent>& event);

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

			EraseSequenceData(eachEvent);
		}
		eventList.clear();
		eventFuncIndexList.clear();
	}

	bool AnimationEventManager::Load(const json& data)
	{
		static auto& pm = particle::ParticleTool_Manager::GetSingletonInstance();

		auto funcSize = data.size();

		std::string fbxName = "";
		std::string animationName = "";
		float frame = 0;
		AnimationEventType type = AnimationEventType::GameObject_ActivateEvent;
		for (int i = 0; i < funcSize; i++)
		{
			type = (AnimationEventType)data[i]["type"];
			fbxName = data[i]["fbxName"];
			animationName = data[i]["animationName"];
			frame = data[i]["frame"];
			
			switch (type)
			{
				case application::AnimationEventType::GameObject_ActivateEvent:
				{
					auto particleName = data[i]["objName"];
					auto ptr = std::make_shared<GameObject_ActivateEvent>();
					ptr->fbxName = fbxName;
					ptr->animationName = animationName;
					ptr->frame = frame;
					ptr->objName = particleName;
					pm.AddAnimationEvent(ptr);
					break;
				}
				case application::AnimationEventType::GameObject_DisabledEvent:
				{
					auto particleName = data[i]["objName"];
					auto ptr = std::make_shared<GameObject_DisabledEvent>();
					ptr->fbxName = fbxName;
					ptr->animationName = animationName;
					ptr->frame = frame;
					ptr->objName = particleName;
					pm.AddAnimationEvent(ptr);
					break;
				}
				case application::AnimationEventType::Sound_PlayOnceEvent:
				{
					auto rscPath = data[i]["rscPath"];
					auto ptr = std::make_shared<Sound_PlayOnceEvent>();
					ptr->fbxName = fbxName;
					ptr->animationName = animationName;
					ptr->frame = frame;
					ptr->rscPath = rscPath;
					pm.AddAnimationEvent(ptr);
					break;
				}
				case application::AnimationEventType::Sound_PlayLoopEvent:
				{
					auto rscPath = data[i]["rscPath"];
					auto ptr = std::make_shared<Sound_PlayLoopEvent>();
					ptr->fbxName = fbxName;
					ptr->animationName = animationName;
					ptr->frame = frame;
					ptr->rscPath = rscPath;
					pm.AddAnimationEvent(ptr);
					break;
				}
				default:
					break;
			}
		}

		ppisLoad = true;

		return true;
	}

	bool AnimationEventManager::Save(json& data)
	{
		for (auto& event : eventList)
		{
			auto type = event->GetType();

			switch (type)
			{
				case application::AnimationEventType::GameObject_ActivateEvent:
				{
					auto ptr = static_cast<GameObject_ActivateEvent*>(event.get());
					json ptrData;
					ptrData["type"] = AnimationEventType::GameObject_ActivateEvent;
					ptrData["fbxName"] = ptr->fbxName;
					ptrData["animationName"] = ptr->animationName;
					ptrData["frame"] = ptr->frame;
					ptrData["objName"] = ptr->objName;
					data.push_back(ptrData);
					break;
				}
				case application::AnimationEventType::GameObject_DisabledEvent:
				{
					auto ptr = static_cast<GameObject_DisabledEvent*>(event.get());
					json ptrData;
					ptrData["type"] = AnimationEventType::GameObject_DisabledEvent;
					ptrData["fbxName"] = ptr->fbxName;
					ptrData["animationName"] = ptr->animationName;
					ptrData["frame"] = ptr->frame;
					ptrData["objName"] = ptr->objName;
					data.push_back(ptrData);
					break;
				}
				case application::AnimationEventType::Sound_PlayOnceEvent:
				{
					auto ptr = static_cast<Sound_PlayOnceEvent*>(event.get());
					json ptrData;
					ptrData["type"] = AnimationEventType::Sound_PlayOnceEvent;
					ptrData["fbxName"] = ptr->fbxName;
					ptrData["animationName"] = ptr->animationName;
					ptrData["frame"] = ptr->frame;
					ptrData["rscPath"] = ptr->rscPath;
					data.push_back(ptrData);
					break;
				}
				case application::AnimationEventType::Sound_PlayLoopEvent:
				{
					auto ptr = static_cast<Sound_PlayLoopEvent*>(event.get());
					json ptrData;
					ptrData["type"] = AnimationEventType::Sound_PlayLoopEvent;
					ptrData["fbxName"] = ptr->fbxName;
					ptrData["animationName"] = ptr->animationName;
					ptrData["frame"] = ptr->frame;
					ptrData["rscPath"] = ptr->rscPath;
					data.push_back(ptrData);
					break;
				}
				default:
					break;
			}
		}
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

		EraseSequenceData(event);

		eventList.erase(event);
		eventFuncIndexList.erase(event);

		return true;
	}
}
