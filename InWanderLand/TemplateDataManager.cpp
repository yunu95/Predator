#include "TemplateDataManager.h"

#include "TemplateDataList.h"

namespace application
{
	namespace editor
	{
		TemplateDataManager::TemplateDataManager()
			: Storable(), Singleton<TemplateDataManager>(), list(), typeMap(), uuidKeyMap(), ptrKeyMap(), dataContainer()
		{
			// 대략 100개 정도의 데이터를 수용할 수 있도록 미리 할당함
			dataContainer.reserve(100);
		}

		ITemplateData* TemplateDataManager::CreateTemplateData(const std::string& name, const DataType& type)
		{
			if (list.find(name) != list.end())
			{
				// 이미 해당 이름으로 생성된 ITemplateData 가 있음
				return nullptr;
			}

			ITemplateData* instance = nullptr;

			switch (type)
			{
				case DataType::TerrainData:
				{
					instance = new Terrain_TemplateData;
					break;
				}

				case DataType::UnitData:
				{
					instance = new Unit_TemplateData;
					break;
				}

				case DataType::OrnamentData:
				{
					instance = new Ornament_TemplateData;
					break;
				}

				default:
					break;
			}

			if (instance != nullptr)
			{
				list[name] = std::unique_ptr<ITemplateData>(instance);
				typeMap[instance] = type;
				uuidKeyMap[instance->id] = name;
				ptrKeyMap[instance] = name;
			}

			return instance;
		}

		ITemplateData* TemplateDataManager::CloneTemplateData(const std::string& name, const ITemplateData* prototype)
		{
			if (list.find(name) != list.end())
			{
				// 이미 해당 이름으로 생성된 ITemplateData 가 있음
				return nullptr;
			}

			DataType type = typeMap[prototype];

			auto instance = CreateTemplateData(name, type);

			if (instance != nullptr)
			{
				switch (type)
				{
					case DataType::TerrainData:
					{
						*static_cast<Terrain_TemplateData*>(instance) = *static_cast<Terrain_TemplateData*>(const_cast<ITemplateData*>(prototype));
						break;
					}

					case DataType::UnitData:
					{
						*static_cast<Unit_TemplateData*>(instance) = *static_cast<Unit_TemplateData*>(const_cast<ITemplateData*>(prototype));
						break;
					}

					case DataType::OrnamentData:
					{
						*static_cast<Ornament_TemplateData*>(instance) = *static_cast<Ornament_TemplateData*>(const_cast<ITemplateData*>(prototype));
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
			auto itr = list.find(name);
			if (itr == list.end())
			{
				// 해당 이름으로 생성된 ITemplateData 가 없으면 nullptr 반환
				return nullptr;
			}

			return itr->second.get();
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
			auto itr = typeMap.find(GetTemplateData(GetDataKey(uuid)));
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

		bool TemplateDataManager::PreDecoding(const json& data)
		{
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
