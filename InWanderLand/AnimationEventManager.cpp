#include "AnimationEventManager.h"

#include "YunutyEngine.h"
#include "ParticleTool_Manager.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "SFXManager.h"

#include <cmath>

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

		container = Scene::getCurrentScene()->AddGameObject();
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

			if (eachEvent->GetType() != application::AnimationEventType::GameObject_TransformEditEvent)
			{
				animator->EraseAnimationFunc(ani, eventFuncIndexList[eachEvent]);
				eventFuncIndexList.erase(eachEvent);
			}
			else
			{
				for (auto& each : animationEditFuncList[eachEvent])
				{
					animator->EraseAnimationFunc(ani, each);
				}
				animationEditFuncList.erase(eachEvent);
			}
		}
		eventList.clear();
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
				case application::AnimationEventType::GameObject_TransformEditEvent:
				{
					auto particleName = data[i]["objName"];
					auto ptr = std::make_shared<GameObject_TransformEditEvent>();
					ptr->fbxName = fbxName;
					ptr->animationName = animationName;
					ptr->frame = frame;
					ptr->objName = particleName;
					ptr->editData = data[i]["editData"];
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
				case application::AnimationEventType::GameObject_AwakeEvent:
				{
					auto particleName = data[i]["objName"];
					auto ptr = std::make_shared<GameObject_AwakeEvent>();
					ptr->fbxName = fbxName;
					ptr->animationName = animationName;
					ptr->frame = frame;
					ptr->objName = particleName;
					pm.AddAnimationEvent(ptr);
					break;
				}
				default:
					break;
			}
		}

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
				case application::AnimationEventType::GameObject_TransformEditEvent:
				{
					auto ptr = static_cast<GameObject_TransformEditEvent*>(event.get());
					json ptrData;
					ptrData["type"] = AnimationEventType::GameObject_TransformEditEvent;
					ptrData["fbxName"] = ptr->fbxName;
					ptrData["animationName"] = ptr->animationName;
					ptrData["frame"] = ptr->frame;
					ptrData["objName"] = ptr->objName;
					ptrData["editData"] = ptr->editData;
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
				case application::AnimationEventType::GameObject_AwakeEvent:
				{
					auto ptr = static_cast<GameObject_AwakeEvent*>(event.get());
					json ptrData;
					ptrData["type"] = AnimationEventType::GameObject_AwakeEvent;
					ptrData["fbxName"] = ptr->fbxName;
					ptrData["animationName"] = ptr->animationName;
					ptrData["frame"] = ptr->frame;
					ptrData["objName"] = ptr->objName;
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

				funcIndex = animator->PushAnimationWithFunc(ani, event->frame, [=]()
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
			case application::AnimationEventType::GameObject_TransformEditEvent:
			{
				/// 특별히 따로 관리합니다.
				break;
			}
			case application::AnimationEventType::Sound_PlayOnceEvent:
			{
				auto ptr = static_cast<Sound_PlayOnceEvent*>(event.get());
				funcIndex = animator->PushAnimationWithFunc(ani, event->frame, [=]()
					{
						SFXManager::PlaySoundfile3D(ptr->rscPath, animator->GetGameObject()->GetTransform()->GetWorldPosition());
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
			case application::AnimationEventType::GameObject_AwakeEvent:
			{
				auto ptr = static_cast<GameObject_AwakeEvent*>(event.get());
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
						auto ptr = particle->GetComponent<graphics::ParticleRenderer>();
						ptr->Reset();
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
			default:
				break;
		}

		eventList.insert(event);

		return true;
	}

	bool AnimationEventManager::EraseAnimationEvent(const std::shared_ptr<AnimationEvent>& event)
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

		if (event->GetType() != application::AnimationEventType::GameObject_TransformEditEvent)
		{
			animator->EraseAnimationFunc(ani, eventFuncIndexList[event]);
			eventFuncIndexList.erase(event);
		}
		else
		{
			for (auto& each : animationEditFuncList[event])
			{
				animator->EraseAnimationFunc(ani, each);
			}
			animationEditFuncList.erase(event);
		}

		eventList.erase(event);

		return true;
	}

	bool AnimationEventManager::EditAnimationEventFrame(const std::shared_ptr<AnimationEvent>& event)
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

		animator->EraseAnimationFunc(ani, eventFuncIndexList[event]);
		eventFuncIndexList.erase(event);

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

				funcIndex = animator->PushAnimationWithFunc(ani, event->frame, [=]()
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
			case application::AnimationEventType::GameObject_TransformEditEvent:
			{
				return false;
			}
			case application::AnimationEventType::Sound_PlayOnceEvent:
			{
				auto ptr = static_cast<Sound_PlayOnceEvent*>(event.get());
				funcIndex = animator->PushAnimationWithFunc(ani, event->frame, [=]()
					{
						SFXManager::PlaySoundfile3D(ptr->rscPath, animator->GetGameObject()->GetTransform()->GetWorldPosition());
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
			case application::AnimationEventType::GameObject_AwakeEvent:
			{
				auto ptr = static_cast<GameObject_AwakeEvent*>(event.get());
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
						auto ptr = particle->GetComponent<graphics::ParticleRenderer>();
						ptr->Reset();
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
			default:
				break;
		}

		return true;
	}

	void AnimationEventManager::UpdateTransformEditEvent(const std::shared_ptr<AnimationEvent>& event)
	{
		if (event->GetType() != AnimationEventType::GameObject_TransformEditEvent)
		{
			return;
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
			return;
		}

		for (auto& each : animationEditFuncList[event])
		{
			animator->EraseAnimationFunc(ani, each);
		}

		animationEditFuncList[event].clear();

		auto tsEvent = static_cast<GameObject_TransformEditEvent*>(event.get());

		GameObject* particle = nullptr;
		for (auto& each : (*particleInstanceList)[event->fbxName])
		{
			if (each->name == tsEvent->objName)
			{
				particle = (*particleInstanceIDMap)[each];
				break;
			}
		}

		for (int i = 0; i < ani->GetTotalFrame(); i++)
		{
			unsigned long long funcIndex = animator->PushAnimationWithFunc(ani, i, [=]()
				{
					auto target = GetLerpPoint(tsEvent->editData, i);
					particle->GetTransform()->SetLocalPosition(target->GetLocalPosition());
					particle->GetTransform()->SetLocalRotation(target->GetLocalRotation());
					particle->GetTransform()->SetLocalScale(target->GetLocalScale());
				});

			if (funcIndex == 0)
			{
				return;
			}
			else
			{
				animationEditFuncList[event].push_back(funcIndex);
			}
		}

		return;
	}

	static float smoothstep(float edge0, float edge1, float x)
	{
		x = ImClamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return x * x * (3 - 2 * x);
	}

	Transform* AnimationEventManager::GetLerpPoint(const RampEdit& data, float frame)
	{
		ImVec2 max = ImVec2(1.0f, 1.0f);
		ImVec2 min = ImVec2(0.0f, 0.0f);
		ImVec2 range = ImVec2(2.0f, 1.0f);

		int subStepCount = 20;
		float step = 1.f / float(subStepCount - 1);

		Vector3d pos = Vector3d();
		Vector3d rot = Vector3d();
		Vector3d scal = Vector3d(1, 1, 1);

		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < data.mPointCount[i] - 1; j++)
			{
				const ImVec2 p1 = data.mPts[i][j];
				const ImVec2 p2 = data.mPts[i][j + 1];

				if (frame < p1.x || frame > p2.x)
				{
					continue;
				}

				for (size_t substep = 0; substep < subStepCount - 1; substep++)
				{
					float t = float(substep) * step;

					const ImVec2 sp1 = ImLerp(p1, p2, t);
					const ImVec2 sp2 = ImLerp(p1, p2, t + step);

					const float rt1 = smoothstep(p1.x, p2.x, sp1.x);
					const float rt2 = smoothstep(p1.x, p2.x, sp2.x);

					const ImVec2 pos1 = ImVec2(sp1.x, ImLerp(p1.y, p2.y, rt1));
					const ImVec2 pos2 = ImVec2(sp2.x, ImLerp(p1.y, p2.y, rt2));

					float rate = (pos2.y - pos1.y) / (pos2.x - pos1.x);

					/// TransformEditEvent 의 경우, ImVec2.y 0.5 값을 기준으로 설정하며,
					/// 해당 Object 의 현재 Transform 은 반영하지 않습니다.
					/// Position 은 30 / 0 / -30,
					/// Rotation 은 +360 / 0 / -360,
					/// Scale 은 100 / 1 / 0.01 값을 offset 으로 정합니다.
					if (frame >= pos1.x && frame <= pos2.x)
					{
						switch (i)
						{
							case 0:
							{
								pos.x = ((frame - pos1.x) * rate + pos1.y) * 60 - 30;
								break;
							}
							case 1:
							{
								pos.y = ((frame - pos1.x) * rate + pos1.y) * 60 - 30;
								break;
							}
							case 2:
							{
								pos.z = ((frame - pos1.x) * rate + pos1.y) * 60 - 30;
								break;
							}
							case 3:
							{
								rot.x = ((frame - pos1.x) * rate + pos1.y) * 720 - 360;
								break;
							}
							case 4:
							{
								rot.y = ((frame - pos1.x) * rate + pos1.y) * 720 - 360;
								break;
							}
							case 5:
							{
								rot.z = ((frame - pos1.x) * rate + pos1.y) * 720 - 360;
								break;
							}
							case 6:
							{
								scal.x = pow(10, 4 * ((frame - pos1.x) * rate + pos1.y) - 2);
								break;
							}
							case 7:
							{
								scal.y = pow(10, 4 * ((frame - pos1.x) * rate + pos1.y) - 2);
								break;
							}
							case 8:
							{
								scal.z = pow(10, 4 * ((frame - pos1.x) * rate + pos1.y) - 2);
								break;
							}
							default:
								break;
						}
						continue;
					}
				}
			}
		}
		container->GetTransform()->SetLocalPosition(pos);
		container->GetTransform()->SetLocalRotation(Quaternion(rot));
		container->GetTransform()->SetLocalScale(scal);

		return container->GetTransform();
	}
}
