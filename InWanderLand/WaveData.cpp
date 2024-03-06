#include "WaveData.h"
#include "UnitData.h"
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
        };
        void WaveData::ApplyAsPlaytimeObject()
        {
            ApplyMapAsPod();
        }
        void WaveData::InsertUnitData(WaveUnitData waveUnitData)
        {
            waveUnitDataMap[waveUnitData.unitData] = waveUnitData;
        }
        void WaveData::DeleteUnitData(UnitData* unitData)
        {
            waveUnitDataMap.erase(unitData);
        }
        void WaveData::HideWaveUnitsVisibility()
        {
            for (auto each : waveUnitDataMap)
            {
                each.first->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
            }
        }

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
        bool WaveData::PostLoadCallback()
        {
            ApplyPodAsMap();
            HideWaveUnitsVisibility();

            return true;
        }
        bool WaveData::PreSaveCallback()
        {
            ApplyMapAsPod();

            return true;
        }
        void WaveData::ApplyPodAsMap()
        {
            for (int i = 0; i < pod.waveUnitUUIDS.size(); i++)
            {
                UnitData* unit = dynamic_cast<UnitData*>(InstanceManager::GetSingletonInstance().GetInstance(String_To_UUID(pod.waveUnitUUIDS[i])));
                assert(unit && "wave has uuids but there is no corresponding unitdata!");
                int waveIdx = 0;
                for (int j = 0; j < pod.waveSizes.size(); j++)
                {
                    if (i < pod.waveSizes[j])
                    {
                        waveIdx = j;
                        break;
                    }
                }
                waveUnitDataMap[unit] = { .unitData = unit,.waveIdx = waveIdx,.delay = pod.waveDelays[i] };
            }
        }
        void WaveData::ApplyMapAsPod()
        {
            vector<WaveUnitData> waveUnitDatas;
            for (auto each : waveUnitDataMap)
            {
                waveUnitDatas.push_back(each.second);
            }
            sort(waveUnitDatas.begin(), waveUnitDatas.end(), [](const WaveUnitData& a, const WaveUnitData& b) {
                if (a.waveIdx < b.waveIdx) {
                    return true;
                }

                if (a.waveIdx > b.waveIdx) {
                    return false;
                }

                return a.delay < b.delay; });

            pod.waveUnitUUIDS.clear();
            pod.waveDelays.clear();
            for (auto& eachSize : pod.waveSizes)
                eachSize = 0;
            for (auto each : waveUnitDatas)
            {
                pod.waveUnitUUIDS.push_back(UUID_To_String(each.unitData->GetUUID()));
                pod.waveDelays.push_back(each.delay);
                pod.waveSizes[each.waveIdx]++;
            }
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