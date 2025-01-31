#include "InWanderLand.h"
#include "InstanceManager.h"

namespace application
{
	namespace editor
	{
		InstanceManager::InstanceManager()
			: Storable(), Singleton<InstanceManager>(), templateDataManager(TemplateDataManager::GetSingletonInstance())
			, list(), tdMap(), listBeforeMatching(), mould(nullptr)
		{

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
			for (auto& [idx, each] : list)
			{
				if (each->GetPaletteInstance())
				{
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(false);
				}
			}

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
				if (!dynamic_cast<TerrainData*>(each.second.get()))
					each.second->ApplyAsPlaytimeObject();
			}
			for (auto& each : list)
			{
				each.second->PostApplyAsPlaytimeObject();
			}
		}

		void InstanceManager::ClearPlaytimeObjects()
		{

		}

		void InstanceManager::EnterDataFromGlobalConstant()
		{
			for (auto& each : list)
			{
				each.second->EnterDataFromGlobalConstant();
			}
		}

		std::unordered_map<const UUID, OrnamentData*>& InstanceManager::GetPunrealGUIDMap()
		{
			return punrealGUIDMap;
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
			if (!data.contains("InstanceList"))
				return true;
			UUID uuid;
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
			if (!data.contains("InstanceList"))
				return true;
			UUID uuid;
			for (auto& [uuidStr, instanceData] : data["InstanceList"].items())
			{
				uuid = String_To_UUID(uuidStr);

				for (auto each : listBeforeMatching)
				{
					if (each->id == uuid)
					{
						if (instanceData["1_Post"]["POD"].contains("templateData") && !UUIDManager::GetSingletonInstance().GetPointerFromUUID<void*>(String_To_UUID(instanceData["1_Post"]["POD"]["templateData"])))
						{
							delete each;
							continue;
						}

						if (!each->PostDecoding(instanceData["1_Post"]))
						{
							Clear();
							return false;
						}

						list[uuid] = std::shared_ptr<IEditableData>(each);
						tdMap[uuid] = each->GetTemplateData();
						break;
					}
				}
			}
			listBeforeMatching.clear();

			return true;
		}
		bool InstanceManager::PostLoad()
		{
			for (auto& [key, ptr] : list)
			{
				ptr->PostLoadCallback();
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
				case DataType::CameraData:
				{
					instance = new CameraData();
					break;
				}
				case DataType::LightData:
				{
					instance = new LightData();
					break;
				}
				case DataType::ParticleData:
				{
					instance = new ParticleData();
					break;
				}
				case DataType::InteractableData:
				{
					instance = new InteractableData();
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
