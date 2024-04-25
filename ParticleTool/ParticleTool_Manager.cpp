#include "ParticleTool_Manager.h"

#include "ParticleToolData.h"
#include "FileSystem.h"

#include "YunutyEngine.h"

#include <fstream>

namespace application
{
	namespace particle
	{
		void ParticleTool_Manager::Clear()
		{
			currentPPPath = "";
			currentPPIsPath = "";

			SetSelectedFBXData(nullptr);
			SetSelectedParticleData(std::shared_ptr<ParticleToolData>());

			particleList.clear();
			for (auto& [key, val] : particleObjList)
			{
				yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(val);
			}
			particleObjList.clear();

			ClearChildPIs();
		}

		void ParticleTool_Manager::LoadSkinnedFBX()
		{
			const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

			for (auto& each : resourceManager->GetFBXList())
			{
				auto& list = resourceManager->GetFBXAnimationList(each);

				if (!list.empty())
				{
					std::string fbxName;
					fbxName.assign(each.begin(), each.end());

					auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(fbxName);
					obj->setName(fbxName);
					auto anim = obj->GetComponent<graphics::Animator>();
					for (auto& each : list)
					{
						anim->PushAnimation(each);
						aniMap[fbxName].push_back(each);
						std::string aniName;
						aniName.assign(each->GetName().begin(), each->GetName().end());
						aniNameMap[fbxName].push_back(aniName);
					}

					skinnedObjList.insert({fbxName, obj});
					obj->SetSelfActive(false);
				}
			}
		}

		bool ParticleTool_Manager::LoadPP(const std::string& path)
		{
			std::ifstream loadFile{ path };

			if (loadFile.is_open())
			{
				json mapData;
				loadFile >> mapData;

				// Manager 초기화
				Clear();

				auto scene = yunutyEngine::Scene::getCurrentScene();

				auto objSize = mapData.size();
				std::string particleName;

				for (int i = 0; i < objSize; i++)
				{
					particleName = mapData[i]["name"];

					auto newParticle = CreateParticle(particleName);

					auto pptr = newParticle.lock().get();
					*pptr = mapData[i];
					UpdateParticleDataObj(particleName);
				}

				loadFile.close();

				currentPPPath = path;
				return true;
			}
			else
			{
				return false;
			}
		}

		bool ParticleTool_Manager::SavePP(const std::string& path)
		{
			json finalPP;

			for (auto& each : particleList)
			{
				json ppData = *each.second.get();
				finalPP.push_back(ppData);
			}

			if (finalPP.is_null())
			{
				return false;
			}

			std::ofstream saveFile{ path };

			if (saveFile.is_open())
			{
				saveFile << finalPP.dump(4);
				saveFile.close();

				return true;
			}
			else
			{
				return false;
			}
		}

		std::string ParticleTool_Manager::GetCurrentPPPath() const
		{
			return currentPPPath;
		}

		void ParticleTool_Manager::SetCurrentPPPath(const std::string& path)
		{
			currentPPPath = path;
		}

		std::vector<std::weak_ptr<ParticleToolData>>& ParticleTool_Manager::GetParticleList()
		{
			static std::vector<std::weak_ptr<ParticleToolData>> container;
			container.resize(particleList.size());

			int i = 0;
			for (auto& [key, val] : particleList)
			{
				container[i] = val;
				i++;
			}

			return container;
		}

		std::weak_ptr<ParticleToolData> ParticleTool_Manager::CreateParticle(const std::string& name)
		{
			std::shared_ptr<ParticleToolData> returnPtr = nullptr;
			if (particleList.find(name) != particleList.end())
			{
				return returnPtr;
			}

			returnPtr = std::make_shared<ParticleToolData>();
			returnPtr->name = name;
			particleList.insert({ name, returnPtr });

			auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			obj->AddComponent<graphics::ParticleRenderer>();
			obj->SetSelfActive(false);
			particleObjList.insert({ name, obj });
			return returnPtr;
		}

		bool ParticleTool_Manager::EraseParticle(const std::string& name)
		{
			if (particleList.find(name) == particleList.end())
			{
				return false;
			}

			if (selectedParticleData == particleList[name])
			{
				selectedParticleData = nullptr;
			}

			particleList.erase(name);
			yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(particleObjList[name]);
			particleObjList.erase(name);
			return true;
		}

		void ParticleTool_Manager::SetSelectedParticleData(const std::weak_ptr<ParticleToolData>& particleData)
		{
			if (particleData.expired())
			{
				if (selectedParticleData)
				{
					particleObjList[selectedParticleData->name]->SetSelfActive(false);
				}
				selectedParticleData = nullptr;
				return;
			}

			if (selectedParticleData)
			{
				particleObjList[selectedParticleData->name]->SetSelfActive(false);
			}

			particleObjList[particleData.lock()->name]->SetSelfActive(true);
			particleObjList[particleData.lock()->name]->GetComponent<graphics::ParticleRenderer>()->Play();

			selectedParticleData = particleData.lock();
		}

		std::weak_ptr<ParticleToolData> ParticleTool_Manager::GetSelectedParticleData()
		{
			return selectedParticleData;
		}

		bool ParticleTool_Manager::RenameParticleData(const std::weak_ptr<ParticleToolData>& particleData, const std::string& name)
		{
			if (particleData.expired() || particleList.find(name) != particleList.end())
			{
				return false;
			}

			auto key = particleData.lock()->name;

			auto sptr = particleList[key];
			auto obj = particleObjList[key];

			particleList.erase(key);
			particleObjList.erase(key);

			particleList.insert({name, sptr});
			particleObjList.insert({name, obj});

			sptr->name = name;

			return true;
		}

		void ParticleTool_Manager::UpdateParticleDataObj(const std::string& name)
		{
			if (particleList.find(name) == particleList.end())
			{
				return;
			}

			auto pObj = particleObjList[name]->GetComponent<graphics::ParticleRenderer>();
			
			pObj->SetParticleShape((yunutyEngine::graphics::ParticleShape)particleList[name]->shape);
			//pObj->SetParticleMode((yunutyEngine::graphics::ParticleMode)particleList[name]->particleMode);
			pObj->SetLoop(particleList[name]->isLoop);
			pObj->SetLifeTime(particleList[name]->lifeTime);
			pObj->SetSpeed(particleList[name]->speed);
			pObj->SetStartScale(particleList[name]->startScale);
			pObj->SetEndScale(particleList[name]->endScale);
			pObj->SetMaxParticle(particleList[name]->maxParticle);
			//pObj->SetPlayAwake(particleList[name]->playAwake);

			pObj->SetRateOverTime(particleList[name]->rateOverTime);

			//pObj->SetBurstsCount(particleList[name]->burstsCount);
			//pObj->SetInterval(particleList[name]->interval);
		}

		void ParticleTool_Manager::SwitchMode()
		{
			if (isParticleEditMode)
			{
				SetSelectedParticleData(std::shared_ptr<ParticleToolData>());
				isParticleEditMode = false;
			}
			else
			{
				/// 클리어
				SetSelectedFBXData(nullptr);
				// SetSelectedParticleInstanceData();
				isParticleEditMode = true;
			}
		}

		bool ParticleTool_Manager::LoadPPIs(const std::string& path)
		{
			std::ifstream loadFile{ path };

			if (loadFile.is_open())
			{
				json mapData;
				loadFile >> mapData;

				// 대상이 되는 pp 파일이 없을 경우 취소
				if (mapData.find("pp_path") == mapData.end())
				{
					loadFile.close();
					return false;
				}

				if (!LoadPP(mapData["pp_path"]))
				{
					return false;
				}

				json instancesList = mapData["ParticleInstanceList"];

				auto scene = yunutyEngine::Scene::getCurrentScene();

				auto objSize = instancesList.size();
				std::string particleName;

				ClearChildPIs();

				for (int i = 0; i < objSize; i++)
				{
					// 추가 로직
				}

				loadFile.close();

				currentPPIsPath = path;
				return true;
			}
			else
			{
				return false;
			}
		}

		/// 로직 필요
		bool ParticleTool_Manager::SavePPIs(const std::string& path)
		{
			return false;
		}

		std::string ParticleTool_Manager::GetCurrentPPIsPath() const
		{
			return currentPPIsPath;
		}

		void ParticleTool_Manager::SetCurrentPPIsPath(const std::string& path)
		{
			this->currentPPIsPath = path;
		}

		std::vector<yunutyEngine::GameObject*>& ParticleTool_Manager::GetSkinnedFBXList()
		{
			static std::vector<yunutyEngine::GameObject*> container;
			container.resize(skinnedObjList.size());

			int i = 0;
			for (auto& [key, val] : skinnedObjList)
			{
				container[i] = val;
				i++;
			}

			return container;
		}

		void ParticleTool_Manager::SetSelectedFBXData(yunutyEngine::GameObject* fbxObj)
		{
			if (fbxObj == nullptr)
			{
				if (selectedFBXObject)
				{
					selectedFBXObject->SetSelfActive(false);
				}
				selectedFBXObject = nullptr;
				return;
			}

			if (selectedFBXObject)
			{
				selectedFBXObject->SetSelfActive(false);
			}

			fbxObj->SetSelfActive(true);

			selectedFBXObject = fbxObj;
		}

		yunutyEngine::GameObject* ParticleTool_Manager::GetSelectedFBXData()
		{
			return selectedFBXObject;
		}

		void ParticleTool_Manager::ClearChildPIs()
		{
			/// Object 에 붙어있는 instance 들 제거
		}
	}
}
