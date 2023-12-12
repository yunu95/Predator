#include "InstanceManager.h"

#include "EditableDataList.h"
#include "TemplateDataManager.h"

namespace Application
{
    namespace Editor
    {
        std::unique_ptr<InstanceManager> InstanceManager::instance = nullptr;

        InstanceManager& InstanceManager::GetInstance()
        {
			if (instance == nullptr)
			{
				instance = std::unique_ptr<InstanceManager>(new InstanceManager());
			}

            return *instance;
        }

        InstanceManager::~InstanceManager()
        {

        }

        bool InstanceManager::CreateInstance(const std::string& dataName)
        {
            auto& sdmanager = TemplateDataManager::GetInstance();
            auto sdptr = sdmanager.GetTemplateData(dataName);
            if (sdptr == nullptr)
            {
                return false;
            }

            std::shared_ptr<IEditableData> instance;

            switch (sdmanager.GetDataType(dataName))
            {
                case IEditableData::DataType::Terrain:
                {
                    instance = std::shared_ptr<IEditableData>(new Terrain(dataName));
                    break;
                }

                case IEditableData::DataType::Units:
                {
                    instance = std::shared_ptr<IEditableData>(new Units(dataName));
                    break;
                }

                case IEditableData::DataType::Ornaments:
                {
                    instance = std::shared_ptr<IEditableData>(new Ornaments(dataName));
                    break;
                }

                default:
                    break;
            }

            list.insert(instance);
            
            return true;
        }

        bool InstanceManager::CloneInstance(const std::shared_ptr<IEditableData>& prototype)
        {
            if (list.find(prototype) == list.end())
            {
                // 관리되고 있는 EditableData 가 아닐 경우, 생성하지 않음
                return false;
            }

            list.insert(prototype->Clone());

            return true;
        }

        bool InstanceManager::PreEncoding(json& data) const
        {
            UUID uuid;
            for (auto& each : list)
            {
                uuid = each->GetUUID();
                json eachData;
                if (!each->PreEncoding(eachData[UUID_To_String(uuid)]))
                {
                    return false;
                }
                data["InstanceList"].push_back(eachData);
            }

            return true;
        }

        bool InstanceManager::PostEncoding(json& data) const
        {
            UUID uuid;
            for (auto& each : list)
            {
                uuid = each->GetUUID();

                for (auto& [key, value] : data["InstanceList"].items())
                {
                    if (uuid == String_From_UUID(key))
                    {
                        if (!each->PostEncoding(value))
                        {
                            return false;
                        }
                        break;
                    }
                }
            }

            return true;
        }

        bool InstanceManager::PreDecoding(const json& data)
        {
            return false;
        }

        bool InstanceManager::PostDecoding(const json& data)
        {
            return false;
        }

        /// private
        InstanceManager::InstanceManager()
        {

        }

        void InstanceManager::Clear()
        {
            list.clear();
            mould = nullptr;
        }

    }
}
