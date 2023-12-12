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

        bool TemplateDataManager::CreateTemplateData(const std::string& name, IEditableData::DataType type)
        {
            if (list.find(name) != list.end())
            {
                // 이미 해당 이름으로 생성된 TemplateData 가 있음
                return false;
            }

            switch (type)
            {
                case IEditableData::DataType::Terrain:
                {
                    list[name] = std::shared_ptr<TemplateData>(new Terrain_TemplateData);
                    break;
                }

                case IEditableData::DataType::Units:
                {
                    list[name] = std::shared_ptr<TemplateData>(new Units_TemplateData);
                    break;
                }

                case IEditableData::DataType::Ornaments:
                {
                    list[name] = std::shared_ptr<TemplateData>(new Ornaments_TemplateData);
                    break;
                }

                default:
                    break;
            }

            typeMap[list[name]] = type;

            return true;
        }

        bool TemplateDataManager::CloneTemplateData(const std::string& name, const std::shared_ptr<TemplateData>& prototype)
        {
            if (list.find(name) != list.end())
            {
                // 이미 해당 이름으로 생성된 TemplateData 가 있음
                return false;
            }

            list[name] = prototype->Clone();
            typeMap[list[name]] = typeMap[prototype];

            return true;
        }

        bool TemplateDataManager::DeleteTemplateData(const std::string& name)
        {
            if (list.find(name) == list.end())
            {
                // 해당 이름으로 생성된 TemplateData 가 없음
                return false;
            }

            typeMap.erase(list[name]);
            list.erase(name);

            return true;
        }

        std::shared_ptr<TemplateData> TemplateDataManager::GetTemplateData(const std::string& name) const
        {
            auto itr = list.find(name);
            if (itr == list.end())
            {
                // 해당 이름으로 생성된 TemplateData 가 없으면 nullptr 반환
                return nullptr;
            }

            return itr->second;
        }

        IEditableData::DataType TemplateDataManager::GetDataType(const std::string& name) const
        {
            auto itr = list.find(name);
            if (itr == list.end())
            {
                // 해당 이름으로 생성된 TemplateData 가 없으면 None 반환
                return IEditableData::DataType::None;
            }

            return typeMap.find(itr->second)->second;
        }

        void TemplateDataManager::Clear()
        {
            typeMap.clear();
            list.clear();
        }

        bool TemplateDataManager::PreEncoding(json& data) const
        {
            return false;
        }

        bool TemplateDataManager::PostEncoding(json& data) const
        {
            return false;
        }

        bool TemplateDataManager::PreDecoding(const json& data)
        {
            return false;
        }

        bool TemplateDataManager::PostDecoding(const json& data)
        {
            return false;
        }

        /// private
        TemplateDataManager::TemplateDataManager()
        {

        }
    }
}
