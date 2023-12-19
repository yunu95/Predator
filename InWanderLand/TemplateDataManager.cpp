#include "TemplateDataManager.h"

#include "TemplateDataList.h"

#include <assert.h>

namespace Application
{
    namespace Editor
    {
        std::unique_ptr<TemplateDataManager> TemplateDataManager::instance = nullptr;

        TemplateDataManager& TemplateDataManager::GetInstance()
        {
			if (instance == nullptr)
			{
				instance = std::unique_ptr<TemplateDataManager>(new TemplateDataManager());
			}

            return *instance;
        }

        TemplateDataManager::~TemplateDataManager()
        {

        }

        TemplateData* TemplateDataManager::CreateTemplateData(const std::string& name, const IEditableData::DataType& type)
        {
            if (list.find(name) != list.end())
            {
                // 이미 해당 이름으로 생성된 TemplateData 가 있음
                return false;
            }

            TemplateData* instance;

            switch (type)
            {
                case IEditableData::DataType::Terrain:
                {
                    instance = new Terrain_TemplateData;
                    break;
                }

                case IEditableData::DataType::Units:
                {
                    instance = new Units_TemplateData;
                    break;
                }

                case IEditableData::DataType::Ornaments:
                {
                    instance = new Ornaments_TemplateData;
                    break;
                }

                default:
                    break;
            }

            list[name] = std::unique_ptr<TemplateData>(instance);
            typeMap[instance] = type;
            uuidKeyMap[instance->GetUUID()] = name;
            ptrKeyMap[instance] = name;

            return instance;
        }

        TemplateData* TemplateDataManager::CloneTemplateData(const std::string& name, const TemplateData* prototype)
        {
            if (list.find(name) != list.end())
            {
                // 이미 해당 이름으로 생성된 TemplateData 가 있음
                return nullptr;
            }

            IEditableData::DataType type = typeMap[prototype];

            auto instance = CreateTemplateData(name, type);

            if (instance != nullptr)
            {
                switch (type)
                {
                    case IEditableData::DataType::Terrain:
                    {
                        *static_cast<Terrain_TemplateData*>(instance) = *static_cast<Terrain_TemplateData*>(const_cast<TemplateData*>(prototype));
                        break;
                    }

                    case IEditableData::DataType::Units:
                    {
                        *static_cast<Units_TemplateData*>(instance) = *static_cast<Units_TemplateData*>(const_cast<TemplateData*>(prototype));
                        break;
                    }

                    case IEditableData::DataType::Ornaments:
                    {
                        *static_cast<Ornaments_TemplateData*>(instance) = *static_cast<Ornaments_TemplateData*>(const_cast<TemplateData*>(prototype));
                        break;
                    }

                    default:
                        break;
                }
            }

            return instance;
        }

        bool TemplateDataManager::DeleteTemplateData(const std::string& name)
        {
            if (list.find(name) == list.end())
            {
                // 해당 이름으로 생성된 TemplateData 가 없음
                return false;
            }

            typeMap.erase(list[name].get());
            uuidKeyMap.erase(list[name].get()->GetUUID());
            ptrKeyMap.erase(list[name].get());
            list.erase(name);

            return true;
        }

        TemplateData* TemplateDataManager::GetTemplateData(const std::string& name) const
        {
            auto itr = list.find(name);
            if (itr == list.end())
            {
                // 해당 이름으로 생성된 TemplateData 가 없으면 nullptr 반환
                return nullptr;
            }

            return itr->second.get();
        }

        IEditableData::DataType TemplateDataManager::GetDataType(const std::string& name) const
        {
            auto itr = list.find(name);
            if (itr == list.end())
            {
                // 해당 이름으로 생성된 TemplateData 가 없으면 None 반환
                return IEditableData::DataType::None;
            }

            return typeMap.find(itr->second.get())->second;
        }

        IEditableData::DataType TemplateDataManager::GetDataType(const TemplateData* ptr) const
        {
            auto itr = typeMap.find(const_cast<TemplateData*>(ptr));
            if (itr == typeMap.end())
            {
                // 해당 데이터가 없으면 None 반환
                return IEditableData::DataType::None;
            }

            return itr->second;
        }

        IEditableData::DataType TemplateDataManager::GetDataType(const UUID& uuid) const
        {
            auto itr = typeMap.find(GetTemplateData(GetDataKey(uuid)));
            if (itr == typeMap.end())
            {
                // 해당 데이터가 없으면 None 반환
                return IEditableData::DataType::None;
            }

            return itr->second;
        }

        std::string TemplateDataManager::GetDataKey(const TemplateData* ptr) const
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

        void TemplateDataManager::Clear()
        {
            typeMap.clear();
            uuidKeyMap.clear();
            ptrKeyMap.clear();
            list.clear();
        }

        bool TemplateDataManager::PreEncoding(json& data) const
        {
            UUID uuid;
            std::string uuidStr;
            for (auto& [key, ptr] : list)
            {
                uuid = ptr->GetUUID();
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
                uuid = ptr->GetUUID();
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

        bool TemplateDataManager::PreDecoding(const json& data)
        {
            for (auto& [key, value] : data["TemplateList"].items())
            {
                for (auto& [uuidStr, templateData] : value.items())
                {
                    if (!CreateTemplateData(templateData["key"], templateData["type"]))
                    {
                        Clear();
                        return false;
                    }

                    if (!list[templateData["key"]]->PreDecoding(templateData["0_Pre"]))
                    {
                        Clear();
                        return false;
                    }

                }
            }
            
            return true;
        }

        bool TemplateDataManager::PostDecoding(const json& data)
        {
            for (auto& [key, value] : data["TemplateList"].items())
            {
                for (auto& [uuidStr, templateData] : value.items())
                {
                    // Change UUID
                    UUID uuid = String_To_UUID(uuidStr);
                    uuidKeyMap.erase(uuid);
                    uuidKeyMap[uuid] = templateData["key"];
                    list[templateData["key"]]->SetUUID(uuid);

                    if (!list[templateData["key"]]->PostDecoding(templateData["1_Post"]))
                    {
                        Clear();
                        return false;
                    }

                }
            }

            return true;
        }

        /// private
        TemplateDataManager::TemplateDataManager()
            : list(), typeMap(), uuidKeyMap(), ptrKeyMap()
        {

        }
    }
}
