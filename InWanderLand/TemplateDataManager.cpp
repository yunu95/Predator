#include "TemplateDataManager.h"

#include "TemplateDataList.h"

namespace application
{
	namespace editor
	{
		TemplateDataManager::TemplateDataManager()
			: Storable(), Singleton<TemplateDataManager>(), list(), typeMap(), uuidKeyMap(), ptrKeyMap()
		{

		}

		ITemplateData* TemplateDataManager::CreateTemplateData(const std::string& name, const IEditableData::DataType& type)
		{
			if (list.find(name) != list.end())
			{
				// 이미 해당 이름으로 생성된 ITemplateData 가 있음
				return nullptr;
			}

			ITemplateData* instance = nullptr;

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

			IEditableData::DataType type = typeMap[prototype];

			auto instance = CreateTemplateData(name, type);

			if (instance != nullptr)
			{
				switch (type)
				{
					case IEditableData::DataType::Terrain:
					{
						*static_cast<Terrain_TemplateData*>(instance) = *static_cast<Terrain_TemplateData*>(const_cast<ITemplateData*>(prototype));
						break;
					}

					case IEditableData::DataType::Units:
					{
						*static_cast<Units_TemplateData*>(instance) = *static_cast<Units_TemplateData*>(const_cast<ITemplateData*>(prototype));
						break;
					}

					case IEditableData::DataType::Ornaments:
					{
						*static_cast<Ornaments_TemplateData*>(instance) = *static_cast<Ornaments_TemplateData*>(const_cast<ITemplateData*>(prototype));
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

		IEditableData::DataType TemplateDataManager::GetDataType(const std::string& name) const
		{
			auto itr = list.find(name);
			if (itr == list.end())
			{
				// 해당 이름으로 생성된 ITemplateData 가 없으면 None 반환
				return IEditableData::DataType::None;
			}

			return typeMap.find(itr->second.get())->second;
		}

		IEditableData::DataType TemplateDataManager::GetDataType(const ITemplateData* ptr) const
		{
			auto itr = typeMap.find(const_cast<ITemplateData*>(ptr));
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
