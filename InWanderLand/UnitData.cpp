#include "InWanderLand.h"
#include "UnitData.h"
#include "UnitPalette.h"
#include "UnitEditorInstance.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"

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
        };

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
            return unitInstance;
        };

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
