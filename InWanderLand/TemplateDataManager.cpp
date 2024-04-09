#include <concepts>
#include "InWanderLand.h"
#include "TemplateDataManager.h"

#include "TemplateDataList.h"

namespace application
{
    namespace editor
    {
        TemplateDataManager::TemplateDataManager()
            : Storable(), Singleton<TemplateDataManager>(), list(), typeMap(), 
            uuidKeyMap(), ptrKeyMap(), dataContainer(), selectedData(nullptr)
        {
            // 대략 100개 정도의 데이터를 수용할 수 있도록 미리 할당함
            dataContainer.reserve(100);
        }

        bool TemplateDataManager::DeleteTemplateData(const std::string& name)
        {
            if (list.find(name) == list.end())
            {
                // 해당 이름으로 생성된 ITemplateData 가 없음
                return false;
            }

            typeMap.erase(list[name].get());
            uuidKeyMap.erase(list[name].get()->id);
            ptrKeyMap.erase(list[name].get());
            list.erase(name);

            return true;
        }

        ITemplateData* TemplateDataManager::GetTemplateData(const std::string& name) const
        {
            return GetTemplateData<ITemplateData>(name);
        }
        DataType TemplateDataManager::GetDataType(const std::string& name) const
        {
            auto itr = list.find(name);
            if (itr == list.end())
            {
                // 해당 이름으로 생성된 ITemplateData 가 없으면 None 반환
                return DataType::None;
            }

            return typeMap.find(itr->second.get())->second;
        }

        DataType TemplateDataManager::GetDataType(const ITemplateData* ptr) const
        {
            auto itr = typeMap.find(const_cast<ITemplateData*>(ptr));
            if (itr == typeMap.end())
            {
                // 해당 데이터가 없으면 None 반환
                return DataType::None;
            }

            return itr->second;
        }

        DataType TemplateDataManager::GetDataType(const UUID& uuid) const
        {
            auto itr = typeMap.find(GetTemplateData<ITemplateData>(GetDataKey(uuid)));
            if (itr == typeMap.end())
            {
                // 해당 데이터가 없으면 None 반환
                return DataType::None;
            }

            return itr->second;
        }

        std::string TemplateDataManager::GetDataKey(const ITemplateData* ptr) const
        {
            auto itr = ptrKeyMap.find(ptr);
            if (itr == ptrKeyMap.end())
            {
                // 해당 데이터가 없다면 빈 string 전달
                return std::string();
            }

            return itr->second;
        }

        std::string TemplateDataManager::GetDataKey(const UUID& uuid) const
        {
            auto itr = uuidKeyMap.find(uuid);
            if (itr == uuidKeyMap.end())
            {
                // 해당 데이터가 없다면 빈 string 전달
                return std::string();
            }

            return itr->second;
        }

        const std::vector<ITemplateData*>& TemplateDataManager::GetDataList(const DataType& type)
        {
            dataContainer.clear();

            for (auto& each : list)
            {
                auto ptr = each.second.get();
                if (typeMap[ptr] == type)
                {
                    dataContainer.push_back(ptr);
                }
            }

            return dataContainer;
        }

        void TemplateDataManager::Clear()
        {
            typeMap.clear();
            uuidKeyMap.clear();
            ptrKeyMap.clear();
            dataContainer.clear();
            list.clear();
            selectedData = nullptr;
        }

        ITemplateData* TemplateDataManager::GetSelectedTemplateData() const
        {
            return selectedData;
        }

        void TemplateDataManager::SetSelectedTemplateData(const ITemplateData* ptr)
        {
            selectedData = const_cast<ITemplateData*>(ptr);
        }

        void TemplateDataManager::EnterDataFromGlobalConstant()
        {
            for (auto& each : list)
            {
                each.second->EnterDataFromGlobalConstant();
            }
        }

        bool TemplateDataManager::PreSave()
        {
            for (auto& [key, ptr] : list)
            {
                ptr->PreSaveCallback();
            }
            return true;
        }

        bool TemplateDataManager::PreEncoding(json& data) const
        {
            UUID uuid;
            std::string uuidStr;
            for (auto& [key, ptr] : list)
            {
                uuid = ptr->id;
                uuidStr = UUID_To_String(uuid);

                data["TemplateList"][uuidStr]["key"] = key;
                data["TemplateList"][uuidStr]["type"] = GetDataType(uuid);
                if (!ptr->PreEncoding(data["TemplateList"][uuidStr]["0_Pre"]))
                {
                    return false;
                }
            }
            return true;
        }

        bool TemplateDataManager::PostEncoding(json& data) const
        {
            UUID uuid;
            std::string uuidStr;
            for (auto& [key, ptr] : list)
            {
                uuid = ptr->id;
                uuidStr = UUID_To_String(uuid);

                auto itr = data["TemplateList"].find(uuidStr);
                if (itr == data["TemplateList"].end())
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

        ITemplateData* TemplateDataManager::CreateTemplateData(const std::string& name, const DataType& type)
        {
            switch (type)
            {
            case DataType::TerrainData:
                return CreateTemplateData<Terrain_TemplateData>(name);
            case DataType::UnitData:
                return CreateTemplateData<Unit_TemplateData>(name);
            case DataType::OrnamentData:
                return CreateTemplateData<Ornament_TemplateData>(name);
            case DataType::RegionData:
                return CreateTemplateData<Region_TemplateData>(name);
            case DataType::WaveData:
                return CreateTemplateData<Wave_TemplateData>(name);
            case DataType::CameraData:
                return CreateTemplateData<Camera_TemplateData>(name);
            case DataType::LightData:
                return CreateTemplateData<Light_TemplateData>(name);
            default:
                return nullptr;
                break;
            }
        }
        bool TemplateDataManager::PreDecoding(const json& data)
        {
            if (!data.contains("TemplateList"))
                return true;
            UUID uuid;
            for (auto& [uuidStr, templateData] : data["TemplateList"].items())
            {
                auto instance = CreateTemplateData(templateData["key"], templateData["type"]);

                if (instance == nullptr)
                {
                    Clear();
                    return false;
                }

                // Change UUID
                uuid = String_To_UUID(uuidStr);
                uuidKeyMap.erase(instance->id);
                uuidKeyMap[uuid] = templateData["key"];
                instance->SetUUID(uuid);

                if (!instance->PreDecoding(templateData["0_Pre"]))
                {
                    Clear();
                    return false;
                }
            }

            return true;
        }

        bool TemplateDataManager::PostDecoding(const json& data)
        {
            if (!data.contains("TemplateList"))
                return true;
            for (auto& [uuidStr, templateData] : data["TemplateList"].items())
            {
                if (!list[templateData["key"]]->PostDecoding(templateData["1_Post"]))
                {
                    Clear();
                    return false;
                }
            }

            return true;
        }
    }
}
