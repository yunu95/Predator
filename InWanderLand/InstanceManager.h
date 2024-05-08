/// 2023. 11. 23 김상준
/// 실제 Instance 가 되어 관리되는 IEditableData 에 대한 Manager 클래스
/// IEditableData 는 InstanceMaanger 를 통해서만 생성할 수 있음

#pragma once

#include "Singleton.h"
#include "Storable.h"
#include "IEditableData.h"
#include "Identifiable.h"
#include "EditableDataList.h"
#include "TemplateDataManager.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace application
{
    namespace editor
    {
        class ITemplateData;
    }
}

namespace application
{
    namespace editor
    {
        class InstanceManager
            : public Storable, public Singleton<InstanceManager>
        {
            friend class Singleton<InstanceManager>;
            friend class MapFileManager;

        public:
            // 이미 만들어진 ITemplateData 로부터 Instance 를 생성함
            template<typename T = IEditableData> requires std::derived_from<T, IEditableData>
            T* CreateInstance(const std::string& dataName)
            {
                auto tdptr = templateDataManager.GetTemplateData(dataName);
                if (tdptr == nullptr)
                {
                    return nullptr;
                }
                std::shared_ptr<T> instance = std::shared_ptr<T>(new T(dataName));

                if (instance != nullptr)
                {
                    list[instance->id] = instance;
                    tdMap[instance->id] = tdptr;
                }

                return instance.get();
            }

            template<>
            IEditableData* CreateInstance<IEditableData>(const std::string& dataName)
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
                case DataType::CameraData:
                {
                    instance = new CameraData(dataName);
                    break;
                }
                case DataType::LightData:
                {
                    instance = new LightData(dataName);
                    break;
                }
                case DataType::ParticleData:
                {
                    instance = new ParticleData(dataName);
                    break;
                }
                default:
                    break;
                }

                if (instance != nullptr)
                {
                    list[instance->id] = std::shared_ptr<IEditableData>(instance);
                    tdMap[instance->id] = tdptr;
                }

                return instance;
            }

            template<typename T = IEditableData> requires std::derived_from<T, IEditableData>
            T* GetInstance(const UUID& uuid) const
            {
                if (auto itr = list.find(uuid); itr != list.end())
                    return dynamic_cast<T*>(itr->second.get());

                return nullptr;
            }
            template<typename T = IEditableData> requires std::derived_from<T, IEditableData>
            std::weak_ptr<T> GetInstanceAsWeakPtr(const UUID& uuid) const
            {
                if (auto itr = list.find(uuid); itr != list.end())
                    return std::weak_ptr<T>(std::dynamic_pointer_cast<T>(itr->second));

                return std::weak_ptr<T>{};
            }
            bool DeleteInstance(const UUID& uuid);
            void Clear();
            void ApplyInstancesAsPlaytimeObjects();
            void ClearPlaytimeObjects();
            void EnterDataFromGlobalConstant();

            template <typename T = IEditableData>
            std::vector<T*> GetList()
            {
                std::vector<T*> returnList;
                for (auto& each : list)
                {
                    auto ptr = dynamic_cast<T*>(each.second.get());
                    if (ptr)
                    {
                        returnList.push_back(ptr);
                    }
                }
                return returnList;
            }

        protected:
            bool PreSave();
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;
            bool PostLoad();

        private:
            InstanceManager();

            // ITemplateData 가 생성되기 전에 그 틀만 우선 작성함
            IEditableData* CreateEmptyInstance(const DataType& type);

            TemplateDataManager& templateDataManager;
            std::unordered_map<const UUID, std::shared_ptr<IEditableData>> list;
            std::unordered_map<const UUID, ITemplateData*> tdMap;
            // CreateEmptyInstance Only
            std::unordered_set<IEditableData*> listBeforeMatching;
            IEditableData* mould;
        };
    }
}
