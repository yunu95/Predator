#include "WaveData.h"
#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "Wave_TemplateData.h"

namespace application
{
    namespace editor
    {
        TemplateDataManager& WaveData::templateDataManager = TemplateDataManager::GetSingletonInstance();
        WaveData* WaveData::selectedEditorWave{nullptr};

        void WaveData::SelectEditingWave(WaveData* waveData)
        {
            selectedEditorWave = waveData;
        }
        bool WaveData::EnterDataFromTemplate()
        {
            return true;
        }

        ITemplateData* WaveData::GetTemplateData()
        {
            return pod.templateData;
        }

        bool WaveData::SetTemplateData(const std::string& dataName)
        {
            return true;
        }

        IEditableData* WaveData::Clone() const
        {
            return nullptr;
        }
        void WaveData::OnRelocate(const Vector3d& newLoc)
        {
        };

        palette::PaletteInstance* WaveData::ApplyAsPaletteInstance()
        {
            return nullptr;
            //if (GetPaletteInstance() == nullptr)
            //{
            //    waveInstance = Scene::getCurrentScene()->AddGameObject()->AddComponent<palette::WaveEditorInstance>();
            //    SetPaletteInstance(waveInstance);
            //    waveInstance->SetEditableData(this);
            //}
            //waveInstance->Apply(this);
            //return waveInstance;
        };

        bool WaveData::PreEncoding(json& data) const
        {
            FieldPreEncoding<boost::pfr::tuple_size_v<POD_Wave>>(pod, data["POD"]);

            return true;
        }

        bool WaveData::PostEncoding(json& data) const
        {
            FieldPostEncoding<boost::pfr::tuple_size_v<POD_Wave>>(pod, data["POD"]);

            return true;
        }

        bool WaveData::PreDecoding(const json& data)
        {
            FieldPreDecoding<boost::pfr::tuple_size_v<POD_Wave>>(pod, data["POD"]);

            return true;
        }

        bool WaveData::PostDecoding(const json& data)
        {
            FieldPostDecoding<boost::pfr::tuple_size_v<POD_Wave>>(pod, data["POD"]);
            return true;
        }

        WaveData::WaveData()
            : pod()
        {
            pod.name = MakeUpName();
        }

        WaveData::WaveData(const std::string& name)
            : IEditableData(), pod()
        {
            pod.name = MakeUpName();
            pod.templateData = static_cast<Wave_TemplateData*>(templateDataManager.GetTemplateData(name));
            EnterDataFromTemplate();
        }

        WaveData::WaveData(const WaveData& prototype)
            : pod(prototype.pod)
        {
            pod.name = MakeUpName();
        }
        std::wstring WaveData::MakeUpName()
        {
            wstringstream wss;
            wss << L"Wave" << GetInstances().size();
            return wss.str();
        }

        WaveData& WaveData::operator=(const WaveData& prototype)
        {
            IEditableData::operator=(prototype);
            pod = prototype.pod;
            return *this;
        }
    }
}