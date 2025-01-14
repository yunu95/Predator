#include "SkillManager.h"

#include "SkillList.h"

namespace application
{
	namespace editor
	{
		bool SkillManager::Skill_Instance::PreEncoding(json& data) const
		{
			return Skill::SkillPodFieldPreEncoding(type, data);
		}

		bool SkillManager::Skill_Instance::PostEncoding(json& data) const
		{
			return Skill::SkillPodFieldPostEncoding(type, data);
		}

		bool SkillManager::Skill_Instance::PreDecoding(const json& data)
		{
			return Skill::SkillPodFieldPreDecoding(type, data);
		}

		bool SkillManager::Skill_Instance::PostDecoding(const json& data)
		{
			return Skill::SkillPodFieldPostDecoding(type, data);
		}

		void SkillManager::Clear()
		{
			/// 예외적으로 재활용하는 방식을 사용합니다.
		}

		bool SkillManager::PreEncoding(json& data) const
		{
			std::string uuidStr;
			for (auto& [uuid, ptr] : list)
			{
				uuidStr = UUID_To_String(uuid);

				data["SkillList"][uuidStr]["type"] = (int)ptr->type;
				if (!ptr->PreEncoding(data["SkillList"][uuidStr]["0_Pre"]))
				{
					return false;
				}
			}

			return true;
		}

		bool SkillManager::PostEncoding(json& data) const
		{
			std::string uuidStr;
			for (auto& [uuid, ptr] : list)
			{
				uuidStr = UUID_To_String(uuid);

				auto itr = data["SkillList"].find(uuidStr);
				if (itr == data["SkillList"].end())
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

		bool SkillManager::PreDecoding(const json& data)
		{
			if (!data.contains("SkillList"))
				return true;
			UUID uuid;
			for (auto& [uuidStr, skillData] : data["SkillList"].items())
			{
				uuid = String_To_UUID(uuidStr);

				for (auto& [tempUuid, instance] : list)
				{
					if (instance->type == (SkillType::Enum)skillData["type"])
					{
						auto sptr = instance;
						list.erase(tempUuid);
						sptr->SetUUID(uuid);
						list[uuid] = sptr;
						break;
					}
				}

				if (!list[uuid]->PreDecoding(skillData["0_Pre"]))
				{
					Clear();
					return false;
				}
			}

			return true;
		}

		bool SkillManager::PostDecoding(const json& data)
		{
			if (!data.contains("SkillList"))
				return true;
			UUID uuid;
			for (auto& [uuidStr, skillData] : data["SkillList"].items())
			{
				uuid = String_To_UUID(uuidStr);

				if (!list[uuid]->PostDecoding(skillData["1_Post"]))
				{
					Clear();
					return false;
				}
			}

			return true;
		}

		SkillManager::SkillManager()
			: list()
		{
			for (int i = 0; i < (int)SkillType::Enum::SKILL_NUM; i++)
			{
				auto temp = std::make_shared<Skill_Instance>();
				temp->type = (SkillType::Enum)i;
				list[temp->GetUUID()] = temp;
			}
		}
	}
}