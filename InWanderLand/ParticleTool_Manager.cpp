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
			ClearPP();
			ClearPPIs();
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

					skinnedObjList.insert({ fbxName, obj });
					obj->SetSelfActive(false);
				}
			}

			/// 그냥 Texture 도 여기서 해버림
			for (auto& textures : resourceManager->GetTextureList())
			{
				if (textures->GetName().find(L"Particle") != std::wstring::npos && textures->GetName().find(L"dds") != std::wstring::npos)
				{
					texturePathList.push_back(std::string(textures->GetName().begin(), textures->GetName().end()));
				}
			}

			aniEventManager.Init();
		}

		bool ParticleTool_Manager::LoadPP(const std::string& path)
		{
			std::ifstream loadFile{ path };

			if (loadFile.is_open())
			{
				json mapData;
				loadFile >> mapData;

				ClearPP();

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

				SetCurrentPPPath(path);
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
				SetSelectedParticleData(std::shared_ptr<ParticleToolData>());
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

			particleList.insert({ name, sptr });
			particleObjList.insert({ name, obj });

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
			pObj->SetParticleMode((yunutyEngine::graphics::ParticleMode)particleList[name]->particleMode);
			pObj->SetLoop(particleList[name]->isLoop);
			pObj->SetDuration(particleList[name]->duration);
			pObj->SetLifeTime(particleList[name]->lifeTime);
			pObj->SetSpeed(particleList[name]->speed);
			pObj->SetStartScale(particleList[name]->startScale);
			pObj->SetEndScale(particleList[name]->endScale);
			pObj->SetMaxParticle(particleList[name]->maxParticle);
			pObj->SetPlayAwake(particleList[name]->playAwake);
			pObj->SetRadius(particleList[name]->radius);
			pObj->SetAngle(particleList[name]->angle);

			pObj->SetRateOverTime(particleList[name]->rateOverTime);

			pObj->SetBurstsCount(particleList[name]->burstsCount);
			pObj->SetInterval(particleList[name]->interval);

			static const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

			std::wstring texturePath;
			texturePath.assign(particleList[name]->texturePath.begin(), particleList[name]->texturePath.end());
			auto texturePtr = resourceManager->GetTexture(texturePath);
			if (texturePtr)
			{
				pObj->SetTexture(texturePtr);
			}
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
				SetSelectedFBXData(nullptr);
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

				ClearPPIs();

				auto scene = yunutyEngine::Scene::getCurrentScene();

				auto objSize = mapData["InstanceList"].size();

				for (int i = 0; i < objSize; i++)
				{
					std::string key = mapData["InstanceList"][i]["targetUnit"];
					auto ptr = AddParticleInstance(skinnedObjList[key], "", mapData["InstanceList"][i]["name"]);
					auto sptr = ptr.lock();
					sptr->particleData = mapData["InstanceList"][i]["particleData"];
					sptr->targetUnit = key;
					sptr->offsetPos.x = mapData["InstanceList"][i]["offsetPos"]["x"];
					sptr->offsetPos.y = mapData["InstanceList"][i]["offsetPos"]["y"];
					sptr->offsetPos.z = mapData["InstanceList"][i]["offsetPos"]["z"];
					sptr->rotation.w = mapData["InstanceList"][i]["rotation"]["w"];
					sptr->rotation.x = mapData["InstanceList"][i]["rotation"]["x"];
					sptr->rotation.y = mapData["InstanceList"][i]["rotation"]["y"];
					sptr->rotation.z = mapData["InstanceList"][i]["rotation"]["z"];
					sptr->scale.x = mapData["InstanceList"][i]["scale"]["x"];
					sptr->scale.y = mapData["InstanceList"][i]["scale"]["y"];
					sptr->scale.z = mapData["InstanceList"][i]["scale"]["z"];
					UpdateParticleInstanceDataObj(sptr);
				}

				if (!aniEventManager.Load(mapData["AniEvents"]))
				{
					loadFile.close();
					return false;
				}

				loadFile.close();

				SetCurrentPPIsPath(path);
				return true;
			}
			else
			{
				return false;
			}
		}

		bool ParticleTool_Manager::SavePPIs(const std::string& path)
		{
			json finalPP;

			for (auto& [fbxObj, dataList] : particleInstanceList)
			{
				for (auto& each : dataList)
				{
					json ppData;
					ppData["particleData"] = each->particleData;
					ppData["targetUnit"] = each->targetUnit;
					ppData["name"] = each->name;
					ppData["offsetPos"]["x"] = each->offsetPos.x;
					ppData["offsetPos"]["y"] = each->offsetPos.y;
					ppData["offsetPos"]["z"] = each->offsetPos.z;
					ppData["rotation"]["w"] = each->rotation.w;
					ppData["rotation"]["x"] = each->rotation.x;
					ppData["rotation"]["y"] = each->rotation.y;
					ppData["rotation"]["z"] = each->rotation.z;
					ppData["scale"]["x"] = each->scale.x;
					ppData["scale"]["y"] = each->scale.y;
					ppData["scale"]["z"] = each->scale.z;
					finalPP["InstanceList"].push_back(ppData);
				}
			}

			if (!aniEventManager.Save(finalPP["AniEvents"]))
			{
				return false;
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
					SetSelectedParticleInstanceData(std::shared_ptr<ParticleToolInstance>());
					SetSelectedAnimation(nullptr);
					selectedFBXObject->SetSelfActive(false);
				}
				selectedFBXObject = nullptr;
				return;
			}

			if (selectedFBXObject)
			{
				SetSelectedParticleInstanceData(std::shared_ptr<ParticleToolInstance>());
				SetSelectedAnimation(nullptr);
				selectedFBXObject->SetSelfActive(false);
			}

			fbxObj->SetSelfActive(true);

			selectedFBXObject = fbxObj;
		}

		yunutyEngine::GameObject* ParticleTool_Manager::GetSelectedFBXData()
		{
			return selectedFBXObject;
		}

		std::vector<std::weak_ptr<ParticleToolInstance>>& ParticleTool_Manager::GetChildrenParticleInstanceList(const std::string& parentsName)
		{
			static std::vector<std::weak_ptr<ParticleToolInstance>> container;
			container.resize(particleInstanceList[parentsName].size());

			int i = 0;
			for (auto& each : particleInstanceList[parentsName])
			{
				container[i] = each;
				i++;
			}

			return container;
		}

		std::vector<std::string>& ParticleTool_Manager::GetAnimationNameList(const std::string& fbxName)
		{
			static std::vector<std::string> container;
			container.resize(aniNameMap[fbxName].size());

			int i = 0;
			for (auto& each : aniNameMap[fbxName])
			{
				container[i] = each;
				i++;
			}

			return container;
		}

		yunuGI::IAnimation* ParticleTool_Manager::GetMatchingIAnimation(const std::string& fbxName, const std::string& aniName)
		{
			int i = 0;
			for (auto& each : aniNameMap[fbxName])
			{
				if (each == aniName)
				{
					return aniMap[fbxName][i];
				}
				i++;
			}

			return nullptr;
		}

		void ParticleTool_Manager::SetSelectedAnimation(yunuGI::IAnimation* ani)
		{
			selectedAnimation = ani;
			SetSelectedAnimationEvent(nullptr);
		}

		yunuGI::IAnimation* ParticleTool_Manager::GetSelectedAnimation()
		{
			return selectedAnimation;
		}

		yunutyEngine::GameObject* ParticleTool_Manager::GetParticleToolInstanceObject(const std::weak_ptr<ParticleToolInstance>& ptr)
		{
			if (!particleInstanceIDMap.contains(ptr.lock()))
			{
				return nullptr;
			}

			return particleInstanceIDMap[ptr.lock()];
		}

		std::weak_ptr<ParticleToolInstance> ParticleTool_Manager::AddParticleInstance(yunutyEngine::GameObject* parents, const std::string& templateName, const std::string& instanceName)
		{
			if (!parents)
			{
				return std::shared_ptr<ParticleToolInstance>();
			}

			std::shared_ptr<ParticleToolInstance> ptr = std::make_shared<ParticleToolInstance>();
			ptr->targetUnit = parents->getName();

			if (instanceName.empty())
			{
				auto tempName = "PI_" + std::to_string(particleInstanceCount);
				if (instanceCountNameList.contains(tempName))
				{
					for (auto itr = instanceCountNameList.find(tempName); itr != instanceCountNameList.end(); itr++)
					{
						if (*itr == "PI_" + std::to_string(particleInstanceCount))
						{
							particleInstanceCount++;
						}
						else
						{
							break;
						}
					}
				}
									
				ptr->name = "PI_" + std::to_string(particleInstanceCount);
			}
			else
			{
				ptr->name = instanceName;
			}

			particleInstanceList[parents->getName()].insert(ptr);
			instanceCountNameList.insert(ptr->name);

			auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
			obj->setName(ptr->name);
			obj->AddComponent<graphics::ParticleRenderer>();

			particleInstanceIDMap[ptr] = obj;

			if (!templateName.empty() && particleList.contains(templateName))
			{
				ptr->particleData = *particleList[templateName];
			}

			UpdateParticleInstanceDataObj(ptr);

			obj->SetSelfActive(false);
			obj->SetParent(parents);

			particleInstanceCount++;

			return ptr;
		}

		bool ParticleTool_Manager::EraseParticleInstance(yunutyEngine::GameObject* parents, const std::weak_ptr<ParticleToolInstance>& instance)
		{
			if (parents == nullptr || !particleInstanceList.contains(parents->getName()) || instance.expired())
			{
				return false;
			}

			auto sptr = instance.lock();

			if (!particleInstanceList[parents->getName()].contains(sptr))
			{
				return false;
			}

			if (selectedParticleInstanceData == sptr)
			{
				SetSelectedParticleInstanceData(std::shared_ptr<ParticleToolInstance>());
			}

			yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(particleInstanceIDMap[sptr]);

			EraseLinkedEvent(instance.lock());

			particleInstanceIDMap.erase(sptr);

			instanceCountNameList.erase(sptr->name);
			particleInstanceList[parents->getName()].erase(sptr);
			return true;
		}

		void ParticleTool_Manager::SetSelectedParticleInstanceData(const std::weak_ptr<ParticleToolInstance>& particleInstanceData)
		{
			if (particleInstanceData.expired())
			{
				if (selectedParticleInstanceData)
				{
					particleInstanceIDMap[selectedParticleInstanceData]->SetSelfActive(false);
				}
				selectedParticleInstanceData = nullptr;
				return;
			}

			if (selectedParticleInstanceData)
			{
				particleInstanceIDMap[selectedParticleInstanceData]->SetSelfActive(false);
			}

			particleInstanceIDMap[particleInstanceData.lock()]->SetSelfActive(true);
			particleInstanceIDMap[particleInstanceData.lock()]->GetComponent<graphics::ParticleRenderer>()->Play();

			selectedParticleInstanceData = particleInstanceData.lock();
		}

		std::weak_ptr<ParticleToolInstance> ParticleTool_Manager::GetSelectedParticleInstanceData()
		{
			return selectedParticleInstanceData;
		}

		void ParticleTool_Manager::UpdateParticleInstanceDataObj(const std::weak_ptr<ParticleToolInstance>& instance)
		{
			if (instance.expired() || !particleInstanceIDMap.contains(instance.lock()))
			{
				return;
			}

			auto sptr = instance.lock();

			auto objTS = particleInstanceIDMap[sptr]->GetTransform();
			objTS->SetLocalPosition(sptr->offsetPos);
			objTS->SetLocalRotation(sptr->rotation);
			objTS->SetLocalScale(sptr->scale);

			auto pptr = particleInstanceIDMap[sptr]->GetComponent<graphics::ParticleRenderer>();

			pptr->SetParticleShape((yunutyEngine::graphics::ParticleShape)sptr->particleData.shape);
			pptr->SetParticleMode((yunutyEngine::graphics::ParticleMode)sptr->particleData.particleMode);
			pptr->SetLoop(sptr->particleData.isLoop);
			pptr->SetDuration(sptr->particleData.duration);
			pptr->SetLifeTime(sptr->particleData.lifeTime);
			pptr->SetSpeed(sptr->particleData.speed);
			pptr->SetStartScale(sptr->particleData.startScale);
			pptr->SetEndScale(sptr->particleData.endScale);
			pptr->SetMaxParticle(sptr->particleData.maxParticle);
			pptr->SetPlayAwake(sptr->particleData.playAwake);
			pptr->SetRadius(sptr->particleData.radius);
			pptr->SetAngle(sptr->particleData.angle);

			pptr->SetRateOverTime(sptr->particleData.rateOverTime);

			pptr->SetBurstsCount(sptr->particleData.burstsCount);
			pptr->SetInterval(sptr->particleData.interval);

			static const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

			std::wstring texturePath;
			texturePath.assign(sptr->particleData.texturePath.begin(), sptr->particleData.texturePath.end());
			auto texturePtr = resourceManager->GetTexture(texturePath);
			if (texturePtr)
			{
				pptr->SetTexture(texturePtr);
			}
		}

		void ParticleTool_Manager::PlaySelectedAnimation()
		{
			if (!selectedAnimation)
			{
				return;
			}
			if (selectedParticleInstanceData)
			{
				SetSelectedParticleInstanceData(std::shared_ptr<ParticleToolInstance>());
			}

			for (auto& each : GetAnimationEventList(selectedAnimation))
			{
				if (each.lock()->GetType() == application::AnimationEventType::GameObject_TransformEditEvent)
				{
					aniEventManager.UpdateTransformEditEvent(each.lock());
				}
			}

			auto animator = GetSelectedFBXData()->GetComponent<graphics::Animator>();
			animator->Play(selectedAnimation);
		}

		void ParticleTool_Manager::StopSelectedAnimation()
		{
			if (!selectedAnimation)
			{
				return;
			}
			
			auto animator = GetSelectedFBXData()->GetComponent<graphics::Animator>();
			animator->Pause();

			for (auto& each : particleInstanceList[selectedFBXObject->getName()])
			{
				particleInstanceIDMap[each]->SetSelfActive(false);
			}
		}

		bool ParticleTool_Manager::IsAnimationPlaying()
		{
			if (!selectedFBXObject)
			{
				return false;
			}

			auto animator = GetSelectedFBXData()->GetComponent<graphics::Animator>();
			return (animator->IsPlaying() && animator->GetCurrentFrame() >= 0 && animator->GetCurrentFrame() <= selectedAnimation->GetTotalFrame());
		}

		bool ParticleTool_Manager::AddAnimationEvent(const std::shared_ptr<application::AnimationEvent>& event)
		{
			yunuGI::IAnimation* ani = nullptr;
			int i = 0;
			for (auto& each : aniMap[event->fbxName])
			{
				if (event->animationName == aniNameMap[event->fbxName][i])
				{
					ani = aniMap[event->fbxName][i];
					break;
				}
				i++;
			}

			aniEventMap[ani].insert(event);
			return aniEventManager.AddAnimationEvent(event);
		}

		bool ParticleTool_Manager::EraseAnimationEvent(const std::shared_ptr<application::AnimationEvent>& event)
		{
			yunuGI::IAnimation* ani = nullptr;
			int i = 0;
			for (auto& each : aniMap[event->fbxName])
			{
				if (event->animationName == aniNameMap[event->fbxName][i])
				{
					ani = aniMap[event->fbxName][i];
					break;
				}
				i++;
			}

			if (selectedAniEvent == event)
			{
				SetSelectedAnimationEvent(nullptr);
			}

			aniEventMap[ani].erase(event);
			return aniEventManager.EraseAnimationEvent(event);
		}

		void ParticleTool_Manager::SetSelectedAnimationEvent(const std::shared_ptr<application::AnimationEvent>& event)
		{
			selectedAniEvent = event;
		}

		std::weak_ptr<application::AnimationEvent> ParticleTool_Manager::GetSelectedAnimationEvent()
		{
			return selectedAniEvent;
		}

		std::vector<std::weak_ptr<AnimationEvent>>& ParticleTool_Manager::GetAnimationEventList(yunuGI::IAnimation* ani)
		{
			static std::vector<std::weak_ptr<AnimationEvent>> container;
			container.resize(aniEventMap[ani].size());

			int i = 0;
			for (auto& each : aniEventMap[ani])
			{
				container[i] = each;
				i++;
			}

			return container;
		}

		void ParticleTool_Manager::EditAnimationEventFrame(const std::weak_ptr<AnimationEvent>& event, float frame)
		{
			if (event.lock()->frame == frame)
			{
				return;
			}

			event.lock()->frame = frame;
			aniEventManager.EditAnimationEventFrame(event.lock());
		}

		void ParticleTool_Manager::ClearPP()
		{
			currentPPPath = "";

			for (auto& [key, val] : particleObjList)
			{
				yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(val);
			}

			SetSelectedParticleData(std::shared_ptr<ParticleToolData>());

			particleList.clear();
			particleObjList.clear();
		}

		void ParticleTool_Manager::ClearPPIs()
		{
			currentPPIsPath = "";

			for (auto& [key, val] : particleInstanceList)
			{
				for (auto& each : val)
				{
					yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(particleInstanceIDMap[each]);
				}
			}

			SetSelectedFBXData(nullptr);
			aniEventManager.Clear();

			instanceCountNameList.clear();
			particleInstanceList.clear();
			particleInstanceIDMap.clear();
			aniEventMap.clear();

			particleInstanceCount = 0;
			aniEventCount = 0;
		}

		void ParticleTool_Manager::EraseLinkedEvent(const std::shared_ptr<ParticleToolInstance>& instance)
		{
			std::vector<std::shared_ptr<application::AnimationEvent>> eraseList;
			for (auto& [fbx, ani] : aniMap)
			{
				if (instance->targetUnit != fbx)
				{
					continue;
				}

				for (auto& each : ani)
				{
					if (aniEventMap.contains(each))
					{
						for (auto& event : aniEventMap[each])
						{
							auto type = event->GetType();
							switch (type)
							{
								case application::AnimationEventType::GameObject_ActivateEvent:
								{
									auto ptr = static_cast<GameObject_ActivateEvent*>(event.get());
									if (ptr->objName == instance->name)
									{
										eraseList.push_back(event);
									}
									break;
								}
								case application::AnimationEventType::GameObject_DisabledEvent:
								{
									auto ptr = static_cast<GameObject_DisabledEvent*>(event.get());
									if (ptr->objName == instance->name)
									{
										eraseList.push_back(event);
									}
									break;
								}
								case application::AnimationEventType::GameObject_TransformEditEvent:
								{
									auto ptr = static_cast<GameObject_TransformEditEvent*>(event.get());
									if (ptr->objName == instance->name)
									{
										eraseList.push_back(event);
									}
									break;
								}
								case application::AnimationEventType::GameObject_AwakeEvent:
								{
									auto ptr = static_cast<GameObject_AwakeEvent*>(event.get());
									if (ptr->objName == instance->name)
									{
										eraseList.push_back(event);
									}
									break;
								}
								default:
									break;
							}
						}
					}
				}

				for (auto& each : eraseList)
				{
					EraseAnimationEvent(each);
				}
			}
		}
	}
}
