#include "InWanderLand.h"
#include "OrnamentData.h"
#include "RegionData.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "ShortcutSystem.h"
#include "EditorResourceManager.h"

namespace application
{
    namespace editor
    {
        TemplateDataManager& OrnamentData::templateDataManager = TemplateDataManager::GetSingletonInstance();

        bool OrnamentData::EnterDataFromTemplate()
        {
            // 템플릿으로부터 초기화되는 데이터들 처리 영역	

            return true;
        }

        ITemplateData* OrnamentData::GetTemplateData()
        {
            return pod.templateData;
        }

        bool OrnamentData::SetTemplateData(const std::string& dataName)
        {
            auto ptr = templateDataManager.GetTemplateData(dataName);
            if (ptr == nullptr)
            {
                return false;
            }

            pod.templateData = static_cast<Ornament_TemplateData*>(ptr);
			OnDataResourceChange(pod.templateData->pod.staticFBXName);

            return true;
        }

        IEditableData* OrnamentData::Clone() const
        {
            auto& imanager = InstanceManager::GetSingletonInstance();
            auto instance = imanager.CreateInstance(pod.templateData->GetDataKey());

            if (instance != nullptr)
            {
                static_cast<OrnamentData*>(instance)->pod = pod;
            }

            return instance;
        }

        void OrnamentData::OnRelocate(const Vector3d& newLoc)
        {
            pod.position.x = newLoc.x;
            pod.position.y = newLoc.y;
            pod.position.z = newLoc.z;
        }

        void OrnamentData::OnRerotate(const Quaternion& newRot)
        {
            pod.rotation.x = newRot.x;
            pod.rotation.y = newRot.y;
            pod.rotation.z = newRot.z;
            pod.rotation.w = newRot.w;
        }

        void OrnamentData::OnRescale(const Vector3d& newScale)
        {
            pod.scale.x = newScale.x;
            pod.scale.y = newScale.y;
            pod.scale.z = newScale.z;
        }

		void OrnamentData::OnDataResourceChange(std::string newName)
		{
			// TemplateData 를 유지하고 Resource 만 갱신함
			if (ornamentInstance)
			{
				ornamentInstance->ChangeResource(newName);
				ApplyAsPaletteInstance();
			}
		}

        palette::PaletteInstance* OrnamentData::ApplyAsPaletteInstance()
        {
            if (GetPaletteInstance() == nullptr)
            {
                ornamentInstance = Scene::getCurrentScene()->AddGameObject()->AddComponent<palette::OrnamentEditorInstance>();
                SetPaletteInstance(ornamentInstance);
                ornamentInstance->SetEditableData(this);
                ornamentInstance->Init(this);
                
                auto fbxName = pod.templateData->pod.staticFBXName;

                static auto& List = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetFBXList();
                int idx = 10;
                for (auto each : List)
                {
                    if (fbxName == std::string(each.begin(), each.end()))
                    {
                        ShortcutSystem::Instance().RegisterTriggerFunction(3,
                            [=]() { ornamentInstance->GetGameObject()->SetSelfActive(ShortcutSystem::Instance().GetTriggerSwitch(3)); });
                        ShortcutSystem::Instance().RegisterTriggerFunction(idx,
                            [=]() { ornamentInstance->GetGameObject()->SetSelfActive(!ornamentInstance->GetGameObject()->GetSelfActive()); });
                        break;
                    }
                    idx++;
                }
            }
            ornamentInstance->GetTransform()->SetWorldPosition({ pod.position.x,pod.position.y,pod.position.z });
            ornamentInstance->GetTransform()->SetWorldRotation({ pod.rotation.w, pod.rotation.x, pod.rotation.y, pod.rotation.z });
            ornamentInstance->GetTransform()->SetLocalScale({ pod.scale.x,pod.scale.y,pod.scale.z });
            return ornamentInstance;
        }

        void OrnamentData::ApplyAsPlaytimeObject()
        {
            if (ornamentInstance == nullptr)
            {
                ApplyAsPaletteInstance();
            }
        }

		bool OrnamentData::EnterDataFromGlobalConstant()
		{
			auto& data = GlobalConstant::GetSingletonInstance().pod;
			return true;
		}

		bool OrnamentData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Ornament>>(pod, data["POD"]);

            return true;
        }

        bool OrnamentData::PostEncoding(json& data) const
        {
            FieldPostEncoding<boost::pfr::tuple_size_v<POD_Ornament>>(pod, data["POD"]);

            return true;
        }

        bool OrnamentData::PreDecoding(const json& data)
        {
            FieldPreDecoding<boost::pfr::tuple_size_v<POD_Ornament>>(pod, data["POD"]);

            return true;
        }

        bool OrnamentData::PostDecoding(const json& data)
        {
            FieldPostDecoding<boost::pfr::tuple_size_v<POD_Ornament>>(pod, data["POD"]);
			EnterDataFromGlobalConstant();
#ifdef EDITOR
            ApplyAsPaletteInstance();
#endif
            return true;
        }

        OrnamentData::~OrnamentData()
        {
            for (auto each : RegionData::GetInstances())
            {
                each->EraseDisablingOrnament(this);
            }
        }
        OrnamentData::OrnamentData()
            : pod()
        {

        }

		OrnamentData::OrnamentData(const std::string& name)
			: pod()
		{
			pod.templateData = static_cast<Ornament_TemplateData*>(templateDataManager.GetTemplateData(name));
			EnterDataFromTemplate();
			EnterDataFromGlobalConstant();
		}

        OrnamentData::OrnamentData(const OrnamentData& prototype)
            : pod(prototype.pod)
        {

        }

        OrnamentData& OrnamentData::operator=(const OrnamentData& prototype)
        {
            IEditableData::operator=(prototype);
            pod = prototype.pod;
            return *this;
        }
    }
}
