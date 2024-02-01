/// 2023. 12. 04 김상준
/// ITemplateData 에 대한 생성 및 관리를 위한 매니저 클래스

#pragma once

#include "Singleton.h"
#include "Storable.h"
#include "Identifiable.h"
#include "IEditableData.h"
#include "ITemplateData.h"

#include <memory>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

namespace application
{
    namespace editor
    {
        class TemplateDataManager
            : public Storable, public Singleton<TemplateDataManager>
        {
            friend class Singleton<TemplateDataManager>;
            friend class MapFileManager;

        public:
            ITemplateData* CreateTemplateData(const std::string& name, const DataType& type);
            template<typename T> requires std::derived_from<T, ITemplateData>
            T* CreateTemplateData(const std::string& name)
            {
                if (list.find(name) != list.end())
                {
                    // 이미 해당 이름으로 생성된 ITemplateData 가 있음
                    return nullptr;
                }

                T* instance = new T;

                list[name] = std::unique_ptr<ITemplateData>(instance);
                typeMap[instance] = GetDataTypeEnum<T>();
                uuidKeyMap[instance->id] = name;
                ptrKeyMap[instance] = name;

                return instance;
            }
            template<typename T> requires std::derived_from<T, ITemplateData>
            T* CloneTemplateData(const std::string& name, const T* prototype)
            {
                if (list.find(name) != list.end())
                {
                    // 이미 해당 이름으로 생성된 ITemplateData 가 있음
                    return nullptr;
                }

                DataType type = typeMap[prototype];

                auto instance = CreateTemplateData<T>(name);
                *instance = *prototype;

                return instance;
            }
            bool DeleteTemplateData(const std::string& name);
            template<typename T> requires std::derived_from<T, ITemplateData>
            T* GetTemplateData(const std::string& name) const
            {
                auto itr = list.find(name);
                if (itr == list.end())
                {
                    // 해당 이름으로 생성된 ITemplateData 가 없으면 nullptr 반환
                    return nullptr;
                }

                return dynamic_cast<T*>(itr->second.get());
            }
            ITemplateData* GetTemplateData(const std::string& name) const;
            DataType GetDataType(const std::string& name) const;
            DataType GetDataType(const ITemplateData* ptr) const;
            DataType GetDataType(const UUID& uuid) const;
            std::string GetDataKey(const ITemplateData* ptr) const;
            std::string GetDataKey(const UUID& uuid) const;
            const std::vector<ITemplateData*>& GetDataList(const DataType& type);
            void Clear();

        protected:
            bool PreSave();
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            TemplateDataManager();

            std::map<const std::string, std::unique_ptr<ITemplateData>> list;
            std::unordered_map<const ITemplateData*, DataType> typeMap;
            std::unordered_map<const UUID, std::string> uuidKeyMap;
            std::unordered_map<const ITemplateData*, std::string> ptrKeyMap;
            std::vector<ITemplateData*> dataContainer;
        };
    }
}
