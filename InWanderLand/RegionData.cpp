#include "InWanderLand.h"
#include "RegionData.h"
#include "RegionPalette.h"
#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "Region_TemplateData.h"
#include "PlaytimeRegion.h"

namespace application
{
    namespace editor
    {
        TemplateDataManager& RegionData::templateDataManager = TemplateDataManager::GetSingletonInstance();
        RegionData* RegionData::selectedEditorRegion{nullptr};

        void RegionData::SelectEditingRegion(RegionData* regionData)
        {
            selectedEditorRegion = regionData;
        }
        bool RegionData::EnterDataFromTemplate()
        {
            return true;
        }

        ITemplateData* RegionData::GetTemplateData()
        {
            return pod.templateData;
        }

        bool RegionData::SetTemplateData(const std::string& dataName)
        {
            return true;
        }

        IEditableData* RegionData::Clone() const
        {
            return nullptr;
        }
        void RegionData::OnRelocate(const Vector3d& newLoc)
        {
            pod.x = newLoc.x;
            pod.z = newLoc.z;
            ApplyAsPaletteInstance();
        };

        palette::PaletteInstance* RegionData::ApplyAsPaletteInstance()
        {
            if (GetPaletteInstance() == nullptr)
            {
                regionInstance = Scene::getCurrentScene()->AddGameObject()->AddComponent<palette::RegionEditorInstance>();
                SetPaletteInstance(regionInstance);
                regionInstance->SetEditableData(this);
            }
            regionInstance->Apply(this);
            return regionInstance;
        };

		void RegionData::ApplyAsPlaytimeObject()
		{
            auto regionGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
            playtimeRegion = regionGameObject->AddComponent<PlaytimeRegion>();
            playtimeRegion->regionData = this;
            pod.name;
            regionGameObject->AddComponent<physics::BoxCollider>()->SetHalfExtent({ pod.width, 1.0f, pod.height });

            regionGameObject->GetTransform()->SetWorldPosition(Vector3d(pod.x, 1.0f, pod.z));
            regionGameObject->GetTransform()->SetWorldRotation(Vector3d(0, pod.angle, 0));
        }

		bool RegionData::PreEncoding(json& data) const
        {
            FieldPreEncoding<boost::pfr::tuple_size_v<POD_Region>>(pod, data["POD"]);

            return true;
        }

        bool RegionData::PostEncoding(json& data) const
        {
            FieldPostEncoding<boost::pfr::tuple_size_v<POD_Region>>(pod, data["POD"]);

            return true;
        }

        bool RegionData::PreDecoding(const json& data)
        {
            FieldPreDecoding<boost::pfr::tuple_size_v<POD_Region>>(pod, data["POD"]);

            return true;
        }

        bool RegionData::PostDecoding(const json& data)
        {
            FieldPostDecoding<boost::pfr::tuple_size_v<POD_Region>>(pod, data["POD"]);
#ifdef EDITOR
            ApplyAsPaletteInstance();
#endif
            return true;
        }
        std::wstring RegionData::MakeUpName()
        {
            wstringstream wss;
            wss << L"Region" << GetInstances().size();
            return wss.str();
        }
        RegionData::RegionData()
            : pod()
        {
            pod.name = MakeUpName();
        }

        RegionData::RegionData(const std::string& name)
            : IEditableData(), pod()
        {
            pod.name = MakeUpName();
            pod.templateData = static_cast<Region_TemplateData*>(templateDataManager.GetTemplateData(name));
            EnterDataFromTemplate();
        }

        RegionData::RegionData(const RegionData& prototype)
            : pod(prototype.pod)
        {
            pod.name = MakeUpName();
        }

        RegionData& RegionData::operator=(const RegionData& prototype)
        {
            IEditableData::operator=(prototype);
            pod = prototype.pod;
            return *this;
        }
    }
}