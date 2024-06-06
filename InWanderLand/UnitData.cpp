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
#include "SFXManager.h"

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
            if (pod.isGuide || pod.waveData != nullptr)
            {
                return;
            }
            UnitPool::SingleInstance().Borrow(this);
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
