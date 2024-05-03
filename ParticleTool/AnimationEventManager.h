/// 2024. 04. 28 김상준
/// AnimationEvents 를 통해 Animator 에 적절한 AnimationEvent 를 등록하는 Manager Class 입니다.

#pragma once

#include "Singleton.h"
#include "AnimationEvents.h"
#include "ParticleToolData.h"

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

namespace yunutyEngine
{
	class GameObject;

	namespace graphics
	{
		class Animator;
	}
}

namespace yunuGI
{
	class IAnimation;
}

namespace application
{
	namespace particle
	{
		class ParticleToolData;
		class ParticleToolInstance;
	}
}

namespace application
{
	class AnimationEventManager
		: public Singleton<AnimationEventManager>
	{
		friend class Singleton<AnimationEventManager>;

	public:
		void Init();
		void Clear();

		bool Load(const json& data);
		bool Save(json& data);

		bool AddAnimationEvent(const std::shared_ptr<AnimationEvent>& event);
		bool EraseAnimationEvent(const std::shared_ptr<AnimationEvent>& event);
		
		void UpdateTransformEditEvent(const std::shared_ptr<AnimationEvent>& event);

	private:
		Transform* GetLerpPoint(const RampEdit& data, float frame);
		GameObject* container = nullptr;

		std::map<const std::string, yunutyEngine::GameObject*>* skinnedObjList = nullptr;
		std::map<const std::string, std::unordered_set<std::shared_ptr<particle::ParticleToolInstance>>>* particleInstanceList = nullptr;
		std::map<const std::shared_ptr<particle::ParticleToolInstance>, yunutyEngine::GameObject*>* particleInstanceIDMap = nullptr;
		std::map<const std::string, std::vector<yunuGI::IAnimation*>>* aniMap = nullptr;
		std::map<const std::string, std::vector<std::string>>* aniNameMap = nullptr;

		std::unordered_set<std::shared_ptr<AnimationEvent>> eventList = std::unordered_set<std::shared_ptr<AnimationEvent>>();
		std::unordered_map<std::shared_ptr<AnimationEvent>, unsigned long long> eventFuncIndexList = std::unordered_map<std::shared_ptr<AnimationEvent>, unsigned long long>();
		std::unordered_map<std::shared_ptr<AnimationEvent>, std::vector<unsigned long long>> animationEditFuncList = std::unordered_map<std::shared_ptr<AnimationEvent>, std::vector<unsigned long long>>();
	};
}
