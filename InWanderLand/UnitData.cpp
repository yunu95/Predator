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
#include "RangedEnemyPool.h"
#include "MeleeEnemyPool.h"
#include "GameManager.h"
#include "Application.h"
#include "ContentsLayer.h"
#include "ShortcutSystem.h"

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

        ITemplateData* UnitData::GetTemplateData()
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
            // 함정과 같은 특수 기믹 객체들도 유닛과 유사하게 위치를 지정해주면 되기 때문에 UnitType에 Bomb, Trap, Bbang/th같은
            // 타입을 확장하여 유닛 생성 로직에서 같이 처리할 수 있게 만들 수 있다.
            //UnitClassifier::SingleInstance().SendPODToClassifier(pod);
            pod.waveData->pod.waveUnitUUIDS;
			application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
            /// 2024.03.20 추가
            // 이제 templateData에서 UnitType에 대한 int값을 가져올 수 있다.
            // 이 값을 통해 타입을 분류해 유닛을 배치해보자.
			
            if (pod.waveData == nullptr)
			{
				Vector3d startPosition = Vector3d(pod.position.x, pod.position.y, pod.position.z);

                if (!isSelectorInitialized)
                {
					productorSelector.push_back(&WarriorProductor::Instance());
					/*productorSelector.push_back(&HealerProductor::Instance());
					productorSelector.push_back(&MagicianProductor::Instance());*/
					productorSelector.push_back(&MeleeEnemyProductor::Instance());
					productorSelector.push_back(&RangedEnemyProductor::Instance());
					//productorSelector.push_back(&BossProductor::Instance());
                    isSelectorInitialized = true;
                }

                if (contentsLayer->isStoppedOnce && isSelectorInitialized)
                {
                    for (auto e : productorSelector)
                    {
                        e->SetUnitData();
                    }
                    contentsLayer->isStoppedOnce = false;
                }

				UnitProductor* currentSelectedProductor{ nullptr };

				Unit* unitComponent{ nullptr };

                int tempShortCutIndex = 0;

                if (pod.templateData->pod.skinnedFBXName == "SKM_Monster1")
                {
					currentSelectedProductor = &MeleeEnemyProductor::Instance();
					currentSelectedProductor->MappingUnitData(pod.templateData->pod);
                    MeleeEnemyPool::SingleInstance().SetStageNumber(pod.stage);
                    MeleeEnemyPool::SingleInstance().SetStartPosition(startPosition);
					unitComponent = MeleeEnemyPool::SingleInstance().Borrow()->m_pairUnit;
					tempShortCutIndex = 2;
                }
                else if (pod.templateData->pod.skinnedFBXName == "SKM_Monster2")
                {
                    currentSelectedProductor = &RangedEnemyProductor::Instance();
					currentSelectedProductor->MappingUnitData(pod.templateData->pod);
					RangedEnemyPool::SingleInstance().SetStartPosition(startPosition);
                    unitComponent = RangedEnemyPool::SingleInstance().Borrow()->m_pairUnit;
                    tempShortCutIndex = 2;
                }
                else
                {
					tempShortCutIndex = 1;
                    for (auto& e : productorSelector)
                    {
                        if (e->SelectUnitProductorByFbxName(pod.templateData->pod.skinnedFBXName))
                        {
                            currentSelectedProductor = e;
                            break;
                        }
                    }
                    currentSelectedProductor->MappingUnitData(pod.templateData->pod);
                    unitComponent = currentSelectedProductor->CreateUnit(startPosition);
					contentsLayer->RegisterToEditorObjectVector(unitComponent->GetGameObject());
                }

				/*ShortcutSystem::Instance().RegisterTriggerFunction(ShortcutSystem::KeyIndex::, 
                        [=]() { unitComponent->GetGameObject()->SetSelfActive(!unitComponent->GetGameObject()->GetSelfActive()); });*/
			}
		}

		void UnitData::PostApplyAsPlaytimeObject()
		{
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
