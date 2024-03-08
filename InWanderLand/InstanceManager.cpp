#include "InWanderLand.h"
#include "InstanceManager.h"

#include "EditableDataList.h"
#include "TemplateDataManager.h"

namespace application
{
    namespace editor
    {
        InstanceManager::InstanceManager()
            : Storable(), Singleton<InstanceManager>(), templateDataManager(TemplateDataManager::GetSingletonInstance())
            , list(), tdMap(), listBeforeMatching(), mould(nullptr)
        {

        }

        template<>
        IEditableData* InstanceManager::CreateInstance<IEditableData>(const std::string& dataName)
        {
            auto tdptr = templateDataManager.GetTemplateData(dataName);
            if (tdptr == nullptr)
            {
                return nullptr;
            }

            IEditableData* instance = nullptr;

            switch (templateDataManager.GetDataType(dataName))
            {
            case DataType::TerrainData:
            {
                instance = new TerrainData(dataName);
                break;
            }

            case DataType::UnitData:
            {
                instance = new UnitData(dataName);
                break;
            }

            case DataType::OrnamentData:
            {
                instance = new OrnamentData(dataName);
                break;
            }
            case DataType::RegionData:
            {
                instance = new RegionData(dataName);
                break;
            }
            case DataType::WaveData:
            {
                instance = new WaveData(dataName);
                break;
            }

            default:
                break;
            }

            if (instance != nullptr)
            {
                list[instance->id] = std::unique_ptr<IEditableData>(instance);
                tdMap[instance->id] = tdptr;
            }

            return instance;
        }

        bool InstanceManager::DeleteInstance(const UUID& uuid)
        {
            if (list[uuid].get() == mould)
            {
                mould = nullptr;
            }

            if (list.erase(uuid) == 0)
            {
                return false;
            }

            tdMap.erase(uuid);

            return true;
        }

        void InstanceManager::Clear()
        {
            tdMap.clear();
            listBeforeMatching.clear();
            list.clear();
            mould = nullptr;
        }
        void InstanceManager::ApplyInstancesAsPlaytimeObjects()
        {
            for (auto& each : list)
            {
                if (dynamic_cast<TerrainData*>(each.second.get()))
                    each.second->ApplyAsPlaytimeObject();
            }
            for (auto& each : list)
            {
                each.second->ApplyAsPlaytimeObject();
            }
        }
 
        bool InstanceManager::PreSave()
        {
            for (auto& [key, ptr] : list)
            {
                ptr->PreSaveCallback();
            }
            return true;
        }
        bool InstanceManager::PreEncoding(json& data) const
        {
            std::string uuidStr;
            for (auto& [uuid, ptr] : list)
            {
                uuidStr = UUID_To_String(uuid);

                data["InstanceList"][uuidStr]["type"] = templateDataManager.GetDataType(tdMap.find(uuid)->second);
                if (!ptr->PreEncoding(data["InstanceList"][uuidStr]["0_Pre"]))
                {
                    return false;
                }
            }

            return true;
        }

        bool InstanceManager::PostEncoding(json& data) const
        {
            std::string uuidStr;
            for (auto& [uuid, ptr] : list)
            {
                uuidStr = UUID_To_String(uuid);

                auto itr = data["InstanceList"].find(uuidStr);
                if (itr == data["InstanceList"].end())
                {
                    return false;
                }

                if (!ptr->PostEncoding(itr.value()["1_Post"]))
                {
                    return false;
                }
            }

            return true;
        }

        bool InstanceManager::PreDecoding(const json& data)
        {
            UUID uuid;
            if (!data.contains("InstanceList"))
                return true;
            for (auto& [uuidStr, instanceData] : data["InstanceList"].items())
            {
                uuid = String_To_UUID(uuidStr);

                auto instance = CreateEmptyInstance(instanceData["type"]);

                assert(instance != nullptr && "incorrect type detected, make sure that datas are stored with proper type value");
                if (instance == nullptr)
                {
                    Clear();
                    return false;
                }

                // UUID
                instance->SetUUID(uuid);

                if (!instance->PreDecoding(instanceData["0_Pre"]))
                {
                    Clear();
                    return false;
                }
            }

            return true;
        }

        bool InstanceManager::PostDecoding(const json& data)
        {
            UUID uuid;
            if (!data.contains("InstanceList"))
                return true;
            for (auto& [uuidStr, instanceData] : data["InstanceList"].items())
            {
                uuid = String_To_UUID(uuidStr);

                for (auto each : listBeforeMatching)
                {
                    if (each->id == uuid)
                    {
                        if (!each->PostDecoding(instanceData["1_Post"]))
                        {
                            Clear();
                            return false;
                        }

                        list[uuid] = std::unique_ptr<IEditableData>(each);
                        tdMap[uuid] = each->GetTemplateData();
                        listBeforeMatching.erase(each);
                        break;
                    }
                }
            }

            return true;
        }

        /// private
        IEditableData* InstanceManager::CreateEmptyInstance(const DataType& type)
        {
            IEditableData* instance = nullptr;

            switch (type)
            {
            case DataType::TerrainData:
            {
                instance = new TerrainData();
                break;
            }

            case DataType::UnitData:
            {
                instance = new UnitData();
                break;
            }

            case DataType::OrnamentData:
            {
                instance = new OrnamentData();
                break;
            }
            case DataType::RegionData:
            {
                instance = new RegionData();
                break;
            }
            case DataType::WaveData:
            {
                instance = new WaveData();
                break;
            }
            default:
                break;
            }

            if (instance != nullptr)
            {
                listBeforeMatching.insert(instance);
            }

            return instance;
        }
    }
}
