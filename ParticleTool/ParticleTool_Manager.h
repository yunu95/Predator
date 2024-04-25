/// 2024. 04. 23 김상준
/// Particle Tool 에서 사용할 Manager 클래스입니다.

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

namespace application
{
	template <typename T>
	class Singleton
	{
	public:
		static T& GetSingletonInstance()
		{
			static std::unique_ptr<T> instance = nullptr;
			if (instance == nullptr)
			{
				instance = std::unique_ptr<T>(new T);
			}
			return *instance;
		}

	protected:
		Singleton()
		{

		}

		~Singleton()
		{

		}

		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;
	};
}

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
			friend class ParticleToolData;

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

			/// 어떤 파티클에 대해서 할 것인지 세팅하는 구조로 변경 필요
			std::weak_ptr<ParticleToolInstance> CreateParticleInstance(const std::string& name);
			bool EraseParticleInstance(const std::string& name);

			void SetSelectedParticleInstanceData(const std::weak_ptr<ParticleToolInstance>& particleInstanceData);
			std::weak_ptr<ParticleToolInstance> GetSelectedParticleInstanceData();

			bool RenameParticleInstanceData(const std::weak_ptr<ParticleToolInstance>& particleInstanceData, const std::string& name);
			void UpdateParticleInstanceDataObj(const std::string& name);
			///

		private:
			void ClearChildPIs();

			bool isParticleEditMode = true;
			std::string currentPPPath;
			std::string currentPPIsPath;
			std::map<const std::string, std::shared_ptr<ParticleToolData>> particleList = std::map<const std::string, std::shared_ptr<ParticleToolData>>();
			std::shared_ptr<ParticleToolData> selectedParticleData = nullptr;
			std::map<const std::string, std::vector<std::shared_ptr<ParticleToolInstance>>> particleInstanceList = std::map<const std::string, std::vector<std::shared_ptr<ParticleToolInstance>>>();
			std::shared_ptr<ParticleToolInstance> selectedParticleInstanceData = nullptr;

			std::map<const std::string, yunutyEngine::GameObject*> particleObjList = std::map<const std::string, yunutyEngine::GameObject*>();
			std::map<const std::string, yunutyEngine::GameObject*> skinnedObjList = std::map<const std::string, yunutyEngine::GameObject*>();
			std::map<const std::string, std::vector<yunuGI::IAnimation*>> aniMap = std::map<const std::string, std::vector<yunuGI::IAnimation*>>();
			std::map<const std::string, std::vector<std::string>> aniNameMap = std::map<const std::string, std::vector<std::string>>();
		};
	}
}
