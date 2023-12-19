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

		IEditableData* InstanceManager::CreateInstance(const std::string& dataName)
		{
			auto& tdmanager = TemplateDataManager::GetInstance();
			auto tdptr = tdmanager.GetTemplateData(dataName);
			if (tdptr == nullptr)
			{
				return nullptr;
			}

			IEditableData* instance;

			switch (tdmanager.GetDataType(dataName))
			{
				case IEditableData::DataType::Terrain:
				{
					instance = new Terrain(dataName);
					break;
				}

				case IEditableData::DataType::Units:
				{
					instance = new Units(dataName);
					break;
				}

				case IEditableData::DataType::Ornaments:
				{
					instance = new Ornaments(dataName);
					break;
				}

				default:
					break;
			}

			list[instance->GetUUID()] = std::unique_ptr<IEditableData>(instance);
			tdMap[instance->GetUUID()] = tdptr;

			return instance;
		}

		IEditableData* InstanceManager::GetInstance(const UUID& uuid) const
		{
			auto itr = list.find(uuid);
			if (itr == list.end())
			{
				return nullptr;
			}

			return itr->second.get();
		}

		void InstanceManager::Clear()
		{
			tdMap.clear();
			listBeforeMatching.clear();
			list.clear();
			mould = nullptr;
		}

		bool InstanceManager::PreEncoding(json& data) const
		{
			auto& tdmanager = TemplateDataManager::GetInstance();

			std::string uuidStr;
			for (auto& [uuid, ptr] : list)
			{
				uuidStr = UUID_To_String(uuid);

				data["InstanceList"][uuidStr]["type"] = tdmanager.GetDataType(tdMap.find(uuid)->second);
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
			for (auto& [uuidStr, instanceData] : data["InstanceList"].items())
			{
				uuid = String_To_UUID(uuidStr);

				auto instance = CreateEmptyInstance(instanceData["type"]);

				if (instance == nullptr)
				{
					Clear();
					return false;
				}

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
			auto& tdmanager = TemplateDataManager::GetInstance();

			UUID uuid;
			for (auto& [uuidStr, instanceData] : data["InstanceList"].items())
			{
				uuid = String_To_UUID(uuidStr);

				for (auto each : listBeforeMatching)
				{
					if (each->GetUUID() == uuid)
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
		InstanceManager::InstanceManager()
			: list(), tdMap(), listBeforeMatching(), mould(nullptr)
		{

		}

		IEditableData* InstanceManager::CreateEmptyInstance(const IEditableData::DataType& type)
		{
			IEditableData* instance;

			switch (type)
			{
				case IEditableData::DataType::Terrain:
				{
					instance = new Terrain();
					break;
				}

				case IEditableData::DataType::Units:
				{
					instance = new Units();
					break;
				}

				case IEditableData::DataType::Ornaments:
				{
					instance = new Ornaments();
					break;
				}

				default:
					break;
			}

			listBeforeMatching.insert(instance);

			return instance;
		}
	}
}
