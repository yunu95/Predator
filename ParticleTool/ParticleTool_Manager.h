/// 2024. 04. 23 김상준
/// Particle Tool 에서 사용할 Manager 클래스입니다.

#pragma once

#include "Singleton.h"
#include "AnimationEventManager.h"

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
	namespace particle
	{
		class ParticleTool_Manager
			: public Singleton<ParticleTool_Manager>
		{
			friend class Singleton<ParticleTool_Manager>;

		public:
			void Clear();
			void LoadSkinnedFBX();

			bool LoadPP(const std::string& path);
			bool SavePP(const std::string& path);

			std::string GetCurrentPPPath() const;
			void SetCurrentPPPath(const std::string& path);

			std::vector<std::weak_ptr<ParticleToolData>>& GetParticleList();

			std::weak_ptr<ParticleToolData> CreateParticle(const std::string& name);
			bool EraseParticle(const std::string& name);

			void SetSelectedParticleData(const std::weak_ptr<ParticleToolData>& particleData);
			std::weak_ptr<ParticleToolData> GetSelectedParticleData();

			bool RenameParticleData(const std::weak_ptr<ParticleToolData>& particleData, const std::string& name);
			void UpdateParticleDataObj(const std::string& name);

			void SwitchMode();

			bool LoadPPIs(const std::string& path);
			bool SavePPIs(const std::string& path);

			std::string GetCurrentPPIsPath() const;
			void SetCurrentPPIsPath(const std::string& path);

			std::vector<yunutyEngine::GameObject*>& GetSkinnedFBXList();

			void SetSelectedFBXData(yunutyEngine::GameObject* fbxObj);
			yunutyEngine::GameObject* GetSelectedFBXData();

			std::vector<std::weak_ptr<ParticleToolInstance>>& GetChildrenParticleInstanceList(const std::string& parentsName);
			std::vector<std::string>& GetAnimationNameList(const std::string& fbxName);
			yunuGI::IAnimation* GetMatchingIAnimation(const std::string& fbxName, const std::string& aniName);

			void SetSelectedAnimation(yunuGI::IAnimation* ani);
			yunuGI::IAnimation* GetSelectedAnimation();

			yunutyEngine::GameObject* GetParticleToolInstanceObject(const std::weak_ptr<ParticleToolInstance>& ptr);

			std::weak_ptr<ParticleToolInstance> AddParticleInstance(yunutyEngine::GameObject* parents, const std::string& name = "");
			bool EraseParticleInstance(yunutyEngine::GameObject* parents, const std::weak_ptr<ParticleToolInstance>& instance);

			void SetSelectedParticleInstanceData(const std::weak_ptr<ParticleToolInstance>& particleInstanceData);
			std::weak_ptr<ParticleToolInstance> GetSelectedParticleInstanceData();

			void UpdateParticleInstanceDataObj(const std::weak_ptr<ParticleToolInstance>& instance);

			void PlaySelectedAnimation();
			bool IsAnimationPlaying();

			bool AddAnimationEvent(const std::shared_ptr<application::AnimationEvent>& event);
			bool EraseAnimationEvent(const std::shared_ptr<application::AnimationEvent>& event);

			void SetSelectedAnimationEvent(const std::shared_ptr<application::AnimationEvent>& event);
			std::weak_ptr<application::AnimationEvent> GetSelectedAnimationEvent();

		private:
			void ClearPP();
			void ClearPPIs();

			AnimationEventManager& aniEventManager = AnimationEventManager::GetSingletonInstance();

			bool isParticleEditMode = true;
			unsigned long long particleInstanceCount = 0;
			std::string currentPPPath;
			std::string currentPPIsPath;
			std::map<const std::string, std::shared_ptr<ParticleToolData>> particleList = std::map<const std::string, std::shared_ptr<ParticleToolData>>();
			std::shared_ptr<ParticleToolData> selectedParticleData = nullptr;
			std::map<const std::string, std::unordered_set<std::shared_ptr<ParticleToolInstance>>> particleInstanceList = std::map<const std::string, std::unordered_set<std::shared_ptr<ParticleToolInstance>>>();
			std::map<const std::shared_ptr<ParticleToolInstance>, yunutyEngine::GameObject*> particleInstanceIDMap = std::map<const std::shared_ptr<ParticleToolInstance>, yunutyEngine::GameObject*>();
			std::shared_ptr<ParticleToolInstance> selectedParticleInstanceData = nullptr;
			yunutyEngine::GameObject* selectedFBXObject = nullptr;
			yunuGI::IAnimation* selectedAnimation = nullptr;
			std::shared_ptr<application::AnimationEvent> selectedAniEvent = nullptr;

			std::map<const std::string, yunutyEngine::GameObject*> particleObjList = std::map<const std::string, yunutyEngine::GameObject*>();
			std::map<const std::string, yunutyEngine::GameObject*> skinnedObjList = std::map<const std::string, yunutyEngine::GameObject*>();
			std::map<const std::string, std::vector<yunuGI::IAnimation*>> aniMap = std::map<const std::string, std::vector<yunuGI::IAnimation*>>();
			std::map<const std::string, std::vector<std::string>> aniNameMap = std::map<const std::string, std::vector<std::string>>();
		};
	}
}
