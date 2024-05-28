#include "InWanderLand.h"
#include "UnitData.h"
#include "UnitPalette.h"
#include "UnitEditorInstance.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "Unit.h"
#include "UnitProductor.h"
#include "WarriorProductor.h"
#include "MagicianProductor.h"
#include "HealerProductor.h"
#include "MeleeEnemyProductor.h"
#include "RangedEnemyProductor.h"
#include "BossProductor.h"
#include "SpikeTrapProductor.h"
#include "RangedEnemyPool.h"
#include "MeleeEnemyPool.h"
#include "GameManager.h"
#include "ShortcutSystem.h"
#include "PlaytimeWave.h"
#include "BurnEffect.h"
#include "ParticleTool_Manager.h"
#include "AnimationEventManager.h"

namespace application
{
    namespace editor
    {
        TemplateDataManager& UnitData::templateDataManager = TemplateDataManager::GetSingletonInstance();

        bool UnitData::EnterDataFromTemplate()
        {
            // 템플릿으로부터 초기화되는 데이터들 처리 영역	

            return true;
        }

        Unit_TemplateData* UnitData::GetTemplateData()
        {
            return pod.templateData;
        }

        bool UnitData::SetTemplateData(const std::string& dataName)
        {
            auto ptr = templateDataManager.GetTemplateData(dataName);
            if (ptr == nullptr)
            {
                return false;
            }

            pod.templateData = static_cast<Unit_TemplateData*>(ptr);
            OnDataResourceChange(pod.templateData->pod.skinnedFBXName);

            return true;
        }

        IEditableData* UnitData::Clone() const
        {
            auto& imanager = InstanceManager::GetSingletonInstance();
            auto instance = imanager.CreateInstance(pod.templateData->GetDataKey());

            if (instance != nullptr)
            {
                static_cast<UnitData*>(instance)->pod = pod;
            }

            return instance;
        }

        void UnitData::OnRelocate(const Vector3d& newLoc)
        {
            pod.position.x = newLoc.x;
            pod.position.y = newLoc.y;
            pod.position.z = newLoc.z;
        }

        void UnitData::OnRerotate(const Quaternion& newRot)
        {
            pod.rotation.x = newRot.x;
            pod.rotation.y = newRot.y;
            pod.rotation.z = newRot.z;
            pod.rotation.w = newRot.w;
        }

        void UnitData::OnRescale(const Vector3d& newScale)
        {
            pod.scale.x = newScale.x;
            pod.scale.y = newScale.y;
            pod.scale.z = newScale.z;
        }

        void UnitData::OnDataResourceChange(std::string newName)
        {
            // TemplateData 를 유지하고 Resource 만 갱신함
            if (unitInstance)
            {
                unitInstance->ChangeResource(newName);
                ApplyAsPaletteInstance();
            }
        }

        palette::PaletteInstance* UnitData::ApplyAsPaletteInstance()
        {
            if (GetPaletteInstance() == nullptr)
            {
                unitInstance = Scene::getCurrentScene()->AddGameObject()->AddComponent<palette::UnitEditorInstance>();
                SetPaletteInstance(unitInstance);
                unitInstance->SetEditableData(this);
                unitInstance->Init(this);
            }
            unitInstance->GetTransform()->SetWorldPosition({ pod.position.x,pod.position.y,pod.position.z });
            unitInstance->GetTransform()->SetWorldRotation({ pod.rotation.w, pod.rotation.x,pod.rotation.y,pod.rotation.z });
            unitInstance->GetTransform()->SetLocalScale({ pod.scale.x,pod.scale.y,pod.scale.z });
            return unitInstance;
        };

        void UnitData::ApplyAsPlaytimeObject()
        {
            if (pod.isGuide)
            {
                return;
            }

            // 함정과 같은 특수 기믹 객체들도 유닛과 유사하게 위치를 지정해주면 되기 때문에 UnitType에 Bomb, Trap, Bbang/th같은
            // 타입을 확장하여 유닛 생성 로직에서 같이 처리할 수 있게 만들 수 있다.
            //UnitClassifier::SingleInstance().SendPODToClassifier(pod);
            pod.waveData->pod.waveUnitUUIDS;
            /// 2024.03.20 추가
            // 이제 templateData에서 UnitType에 대한 int값을 가져올 수 있다.
            // 이 값을 통해 타입을 분류해 유닛을 배치해보자.
            Vector3d startPosition = Vector3d(pod.position.x, 0, pod.position.z);
            Quaternion startRotation = Quaternion(pod.rotation.w, pod.rotation.x, pod.rotation.y, pod.rotation.z);

            if (!isSelectorInitialized)
            {
                productorSelector.push_back(&WarriorProductor::Instance());
                productorSelector.push_back(&HealerProductor::Instance());
                productorSelector.push_back(&MagicianProductor::Instance());
                productorSelector.push_back(&MeleeEnemyProductor::Instance());
                productorSelector.push_back(&RangedEnemyProductor::Instance());
                productorSelector.push_back(&SpikeTrapProductor::Instance());
                productorSelector.push_back(&BossProductor::Instance());
                isSelectorInitialized = true;
            }

            if (isSelectorInitialized)
            {
                for (auto e : productorSelector)
                {
                    e->SetUnitData();
                }
            }

            UnitProductor* currentSelectedProductor{ nullptr };

            int tempShortCutIndex = 0;

            if (pod.templateData->pod.skinnedFBXName == "SKM_Monster1")
            {
				currentSelectedProductor = &MeleeEnemyProductor::Instance();
				currentSelectedProductor->MappingUnitData(pod.templateData->pod);
				MeleeEnemyPool::Instance().SetStageNumber(pod.stage);
				MeleeEnemyPool::Instance().SetStartPosition(startPosition);

				if (pod.templateData->pod.isEliteMonster == true)
				{
					inGameUnit = currentSelectedProductor->CreateUnit(startPosition);
					inGameUnit->GetGameObject()->GetComponent<BurnEffect>()->Appear();
				}
				else
					inGameUnit = MeleeEnemyPool::Instance().Borrow()->m_pairUnit;

				tempShortCutIndex = 2;
            }
            else if (pod.templateData->pod.skinnedFBXName == "SKM_Monster2")
            {
                currentSelectedProductor = &RangedEnemyProductor::Instance();
                currentSelectedProductor->MappingUnitData(pod.templateData->pod);
                RangedEnemyPool::Instance().SetStartPosition(startPosition);

				if (pod.templateData->pod.isEliteMonster == true)
				{
					inGameUnit = currentSelectedProductor->CreateUnit(startPosition);
					inGameUnit->GetGameObject()->GetComponent<BurnEffect>()->Appear();
				}
				else
					inGameUnit = RangedEnemyPool::Instance().Borrow()->m_pairUnit;

                tempShortCutIndex = 2;
            }
            else if (pod.templateData->pod.skinnedFBXName == "SKM_HeartQueen")
            {
				currentSelectedProductor = &BossProductor::Instance();
				currentSelectedProductor->MappingUnitData(pod.templateData->pod);
				inGameUnit = currentSelectedProductor->CreateUnit(startPosition);
				tempShortCutIndex = 2;
            }
            else
            {
                tempShortCutIndex = 1;

                if (pod.templateData->pod.skinnedFBXName == "SKM_Robin")
                {
					currentSelectedProductor = &WarriorProductor::Instance();
                }
                else if (pod.templateData->pod.skinnedFBXName == "SKM_Ursula")
                {
					currentSelectedProductor = &MagicianProductor::Instance();
                }
                else if (pod.templateData->pod.skinnedFBXName == "SKM_Hansel")
                {
                    currentSelectedProductor = &HealerProductor::Instance();;
                }
                currentSelectedProductor->MappingUnitData(pod.templateData->pod);
                inGameUnit = currentSelectedProductor->CreateUnit(startPosition);
            }

            inGameUnit->GetTransform()->SetWorldRotation(startRotation);

            if (inGameUnit)
            {
                auto& ptm = particle::ParticleTool_Manager::GetSingletonInstance();
                const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
                auto obj = inGameUnit->GetGameObject();
                auto animator = obj->GetComponent<graphics::Animator>();
                std::wstring fbxNameW;
                fbxNameW.assign(pod.templateData->pod.skinnedFBXName.begin(), pod.templateData->pod.skinnedFBXName.end());

                /// Particle Setting
                for (auto& eachPI : ptm.GetChildrenParticleInstanceList(pod.templateData->pod.skinnedFBXName))
                {
                    auto pObj = obj->AddGameObject();
                    auto sptr = eachPI.lock();
                    pObj->GetTransform()->SetLocalPosition(sptr->offsetPos);
                    pObj->GetTransform()->SetLocalRotation(sptr->rotation);
                    pObj->GetTransform()->SetLocalScale(sptr->scale);
                    pObj->setName(sptr->name);
                    auto pr = pObj->AddComponent<graphics::ParticleRenderer>();
                    pr->SetParticleShape((yunutyEngine::graphics::ParticleShape)sptr->particleData.shape);
                    pr->SetParticleMode((yunutyEngine::graphics::ParticleMode)sptr->particleData.particleMode);
                    pr->SetLoop(sptr->particleData.isLoop);
                    pr->SetDuration(sptr->particleData.duration);
                    pr->SetLifeTime(sptr->particleData.lifeTime);
                    pr->SetSpeed(sptr->particleData.speed);
                    pr->SetStartScale(sptr->particleData.startScale);
                    pr->SetEndScale(sptr->particleData.endScale);
                    pr->SetMaxParticle(sptr->particleData.maxParticle);
                    pr->SetPlayAwake(sptr->particleData.playAwake);
                    pr->SetRadius(sptr->particleData.radius);
                    pr->SetAngle(sptr->particleData.angle);

                    pr->SetRateOverTime(sptr->particleData.rateOverTime);
                    
                    pr->SetBurstsCount(sptr->particleData.burstsCount);
                    pr->SetInterval(sptr->particleData.interval);

                    std::wstring texturePath;
                    texturePath.assign(sptr->particleData.texturePath.begin(), sptr->particleData.texturePath.end());
                    auto texturePtr = resourceManager->GetTexture(texturePath);
                    if (texturePtr)
                    {
                        pr->SetTexture(texturePtr);
                    }

                    pObj->SetSelfActive(false);
                }

                /// Animation Event Setting
                auto& list = resourceManager->GetFBXAnimationList(fbxNameW);
                for (auto& each : list)
                {
                    std::string aniName;
                    aniName.assign(each->GetName().begin(), each->GetName().end());

                    for (auto& eventWeak : ptm.GetAnimationEventList(ptm.GetMatchingIAnimation(pod.templateData->pod.skinnedFBXName, aniName)))
                    {
                        auto event = eventWeak.lock();
                        auto type = event->GetType();
                        switch (type)
                        {
                            case application::AnimationEventType::GameObject_ActivateEvent:
                            {
                                auto ptr = static_cast<GameObject_ActivateEvent*>(event.get());
                                GameObject* particle = nullptr;
                                for (auto& child : obj->GetChildren())
                                {
                                    if (child->getName() == ptr->objName)
                                    {
                                        particle = child;
                                        break;
                                    }
                                }

                                animator->PushAnimationWithFunc(each, event->frame, [=]()
                                    {
                                        particle->SetSelfActive(true);
                                        auto ptr = particle->GetComponent<graphics::ParticleRenderer>();
                                        ptr->Play();
                                    });

                                break;
                            }
                            case application::AnimationEventType::GameObject_DisabledEvent:
                            {
                                auto ptr = static_cast<GameObject_DisabledEvent*>(event.get());
                                GameObject* particle = nullptr;
                                for (auto& child : obj->GetChildren())
                                {
                                    if (child->getName() == ptr->objName)
                                    {
                                        particle = child;
                                        break;
                                    }
                                }

                                animator->PushAnimationWithFunc(each, event->frame, [=]()
                                    {
                                        particle->SetSelfActive(false);
                                    });

                                break;
                            }
                            case application::AnimationEventType::GameObject_TransformEditEvent:
                            {
                                auto ptr = static_cast<GameObject_TransformEditEvent*>(event.get());
                                GameObject* particle = nullptr;
                                for (auto& child : obj->GetChildren())
                                {
                                    if (child->getName() == ptr->objName)
                                    {
                                        particle = child;
                                        break;
                                    }
                                }

                                for (int i = 0; i < each->GetTotalFrame(); i++)
                                {
                                    animator->PushAnimationWithFunc(each, i, [=]()
                                        {
                                            auto& aem = AnimationEventManager::GetSingletonInstance();
                                            auto target = aem.GetLerpPoint(ptr->editData, i);
                                            particle->GetTransform()->SetLocalPosition(target->GetLocalPosition());
                                            particle->GetTransform()->SetLocalRotation(target->GetLocalRotation());
                                            particle->GetTransform()->SetLocalScale(target->GetLocalScale());
                                        });
                                }

                                break;
                            }
                            case application::AnimationEventType::Sound_PlayOnceEvent:
                            {
                                auto ptr = static_cast<Sound_PlayOnceEvent*>(event.get());
                                animator->PushAnimationWithFunc(each, event->frame, [=]()
                                    {
                                        yunutyEngine::SoundSystem::PlaySoundfile3D(ptr->rscPath, animator->GetGameObject()->GetTransform()->GetWorldPosition());
                                    });
                                break;
                            } 
                            case application::AnimationEventType::GameObject_AwakeEvent:
                            {
                                auto ptr = static_cast<GameObject_AwakeEvent*>(event.get());
                                GameObject* particle = nullptr;
                                for (auto& child : obj->GetChildren())
                                {
                                    if (child->getName() == ptr->objName)
                                    {
                                        particle = child;
                                        break;
                                    }
                                }

                                animator->PushAnimationWithFunc(each, event->frame, [=]()
                                    {
                                        auto ptr = particle->GetComponent<graphics::ParticleRenderer>();
						                ptr->Reset();
                                    });

                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
            }
		}

		void UnitData::PostApplyAsPlaytimeObject()
		{
			if (pod.waveData != nullptr)
			{
				inGameUnit->GetGameObject()->SetSelfActive(false);
			}

		}

        bool UnitData::EnterDataFromGlobalConstant()
        {
            auto& data = GlobalConstant::GetSingletonInstance().pod;
            return true;
        }

        bool UnitData::PreEncoding(json& data) const
        {
            FieldPreEncoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

            return true;
        }

        bool UnitData::PostEncoding(json& data) const
        {
            FieldPostEncoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

            return true;
        }

        bool UnitData::PreDecoding(const json& data)
        {
            FieldPreDecoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

            return true;
        }

        bool UnitData::PostDecoding(const json& data)
        {
            FieldPostDecoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);
            EnterDataFromGlobalConstant();
#ifdef EDITOR
            ApplyAsPaletteInstance();
#endif
            return true;
        }

        UnitData::UnitData()
            : pod()
        {

        }

        UnitData::UnitData(const std::string& name)
            : IEditableData(), pod()
        {
            pod.templateData = static_cast<Unit_TemplateData*>(templateDataManager.GetTemplateData(name));
            EnterDataFromTemplate();
            EnterDataFromGlobalConstant();
        }

        UnitData::UnitData(const UnitData& prototype)
            : pod(prototype.pod)
        {

        }

        UnitData& UnitData::operator=(const UnitData& prototype)
        {
            IEditableData::operator=(prototype);
            pod = prototype.pod;
            return *this;
        }
    }
}
