#include "InWanderLand.h"
#include "InteractableData.h"
#include "InteractablePalette.h"
#include "InteractableEditorInstance.h"

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
#include "PawnTrapProductor.h"
#include "BishopTrapProductor.h"
#include "RookTrapProductor.h"
#include "RangedEnemyPool.h"
#include "MeleeEnemyPool.h"
#include "GameManager.h"
#include "ShortcutSystem.h"
#include "PlaytimeWave.h"
#include "BurnEffect.h"
#include "ParticleTool_Manager.h"
#include "AnimationEventManager.h"
#include "InteractableList.h"

#include <algorithm>

namespace application
{
    namespace editor
    {
        TemplateDataManager& InteractableData::templateDataManager = TemplateDataManager::GetSingletonInstance();

        bool InteractableData::EnterDataFromTemplate()
        {
            // 템플릿으로부터 초기화되는 데이터들 처리 영역	

            return true;
        }

        ITemplateData* InteractableData::GetTemplateData()
        {
            return pod.templateData;
        }

        bool InteractableData::SetTemplateData(const std::string& dataName)
        {
            auto ptr = templateDataManager.GetTemplateData(dataName);
            if (ptr == nullptr)
            {
                return false;
            }

            pod.templateData = static_cast<Interactable_TemplateData*>(ptr);
            OnDataResourceChange(pod.templateData->pod.fBXName);

            return true;
        }

        IEditableData* InteractableData::Clone() const
        {
            auto& imanager = InstanceManager::GetSingletonInstance();
            auto instance = imanager.CreateInstance(pod.templateData->GetDataKey());

            if (instance != nullptr)
            {
                static_cast<InteractableData*>(instance)->pod = pod;
            }

            return instance;
        }

        void InteractableData::OnRelocate(const Vector3d& newLoc)
        {
            pod.position.x = newLoc.x;
            pod.position.y = newLoc.y;
            pod.position.z = newLoc.z;
        }

        void InteractableData::OnRerotate(const Quaternion& newRot)
        {
            pod.rotation.x = newRot.x;
            pod.rotation.y = newRot.y;
            pod.rotation.z = newRot.z;
            pod.rotation.w = newRot.w;
        }

        void InteractableData::OnRescale(const Vector3d& newScale)
        {
            pod.scale.x = newScale.x;
            pod.scale.y = newScale.y;
            pod.scale.z = newScale.z;
        }

        void InteractableData::OnDataResourceChange(std::string newName)
        {
            // TemplateData 를 유지하고 Resource 만 갱신함
            if (interactableInstance)
            {
                interactableInstance->ChangeResource(newName);
                ApplyAsPaletteInstance();
            }
        }

        palette::PaletteInstance* InteractableData::ApplyAsPaletteInstance()
        {
            if (GetPaletteInstance() == nullptr)
            {
                interactableInstance = Scene::getCurrentScene()->AddGameObject()->AddComponent<palette::InteractableEditorInstance>();
                SetPaletteInstance(interactableInstance);
                interactableInstance->SetEditableData(this);
                interactableInstance->Init(this);
            }
            interactableInstance->GetTransform()->SetWorldPosition({ pod.position.x,pod.position.y,pod.position.z });
            interactableInstance->GetTransform()->SetWorldRotation({ pod.rotation.w, pod.rotation.x,pod.rotation.y,pod.rotation.z });
            interactableInstance->GetTransform()->SetLocalScale({ pod.scale.x,pod.scale.y,pod.scale.z });
            return interactableInstance;
        };

        void InteractableData::ApplyAsPlaytimeObject()
        {
			Vector3d startPosition = Vector3d(pod.position.x, 0, pod.position.z);
            Quaternion startQuat = Quaternion(pod.rotation.w, pod.rotation.x, pod.rotation.y, pod.rotation.z);
            Vector3d startScale = Vector3d(pod.scale.x, pod.scale.y, pod.scale.z);

            GameObject* obj = nullptr;

            if (pod.templateData->pod.fBXName == "Trigger_Cube" || pod.templateData->pod.fBXName == "Trigger_Sphere")
            {
                obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
            }
            else
            {
                obj = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(pod.templateData->pod.fBXName);

                yunuGI::FBXData* data = nullptr;
                if (graphics::Renderer::SingleInstance().GetResourceManager()->GetFBXData(pod.templateData->pod.fBXName, data))
                {
                    if (data->hasAnimation)
                    {
                        hasAnimation = true;
                    }
                }
            }
    
            if (pod.templateData->pod.fBXName == "Trigger_Cube")
            {
                auto comp = obj->AddComponent<Interactable_TriggerBox>();
                comp->SetDataFromEditorData(*this);
            }
            else if (pod.templateData->pod.fBXName == "Trigger_Sphere")
            {
                auto comp = obj->AddComponent<Interactable_TriggerSphere>();
                comp->SetDataFromEditorData(*this);
            }
            else if (pod.templateData->pod.fBXName == "SM_Chess_Bishop")
            {
                obj->AddComponent<Interactable_ChessBishop>();
            }
            else if (pod.templateData->pod.fBXName == "SM_Chess_Pawn")
            {
                obj->AddComponent<Interactable_ChessPawn>();
            }
            else if (pod.templateData->pod.fBXName == "SM_Chess_Rook")
            {
                obj->AddComponent<Interactable_ChessRook>();
            }
            else if (pod.templateData->pod.fBXName == "SM_Spike01")
            {
                obj->AddComponent<Interactable_SpikeTrap>();
            }

            if (hasAnimation)
            {
                auto& ptm = particle::ParticleTool_Manager::GetSingletonInstance();
                const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
                auto animator = inGameInteractable->GetComponent<graphics::Animator>();
                std::wstring fbxNameW;
                fbxNameW.assign(pod.templateData->pod.fBXName.begin(), pod.templateData->pod.fBXName.end());

                /// Particle Setting
                for (auto& eachPI : ptm.GetChildrenParticleInstanceList(pod.templateData->pod.fBXName))
                {
                    auto pObj = inGameInteractable->AddGameObject();
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

                    for (auto& eventWeak : ptm.GetAnimationEventList(ptm.GetMatchingIAnimation(pod.templateData->pod.fBXName, aniName)))
                    {
                        auto event = eventWeak.lock();
                        auto type = event->GetType();
                        switch (type)
                        {
                            case application::AnimationEventType::GameObject_ActivateEvent:
                            {
                                auto ptr = static_cast<GameObject_ActivateEvent*>(event.get());
                                GameObject* particle = nullptr;
                                for (auto& child : inGameInteractable->GetChildren())
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
                                for (auto& child : inGameInteractable->GetChildren())
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
                                for (auto& child : inGameInteractable->GetChildren())
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
                                for (auto& child : inGameInteractable->GetChildren())
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

        bool InteractableData::EnterDataFromGlobalConstant()
        {
            auto& data = GlobalConstant::GetSingletonInstance().pod;
            return true;
        }

        bool InteractableData::PreEncoding(json& data) const
        {
            FieldPreEncoding<boost::pfr::tuple_size_v<POD_Interactable>>(pod, data["POD"]);

            return true;
        }

        bool InteractableData::PostEncoding(json& data) const
        {
            FieldPostEncoding<boost::pfr::tuple_size_v<POD_Interactable>>(pod, data["POD"]);

            return true;
        }

        bool InteractableData::PreDecoding(const json& data)
        {
            FieldPreDecoding<boost::pfr::tuple_size_v<POD_Interactable>>(pod, data["POD"]);

            return true;
        }

        bool InteractableData::PostDecoding(const json& data)
        {
            FieldPostDecoding<boost::pfr::tuple_size_v<POD_Interactable>>(pod, data["POD"]);
            EnterDataFromGlobalConstant();
#ifdef EDITOR
            ApplyAsPaletteInstance();
#endif
            return true;
        }

        InteractableData::InteractableData()
            : pod()
        {

        }

        InteractableData::InteractableData(const std::string& name)
            : IEditableData(), pod()
        {
            pod.templateData = static_cast<Interactable_TemplateData*>(templateDataManager.GetTemplateData(name));
            EnterDataFromTemplate();
            EnterDataFromGlobalConstant();
        }

        InteractableData::InteractableData(const InteractableData& prototype)
            : pod(prototype.pod)
        {

        }

        InteractableData& InteractableData::operator=(const InteractableData& prototype)
        {
            IEditableData::operator=(prototype);
            pod = prototype.pod;
            return *this;
        }
    }
}
