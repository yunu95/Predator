#include "ScriptSystem.h"

#include "YunutyEngine.h"

namespace application
{
	void ScriptSystem::SingletonInitializer()
	{

	}

	Script* ScriptSystem::CreateScript()
	{
		auto sptr = Scene::getCurrentScene()->AddGameObject()->AddComponent<Script>();
		scriptList[sptr->GetUUID()] = sptr;
		return sptr;
	}

	bool ScriptSystem::EraseScript(const Script*& script)
	{
		auto itr = scriptList.find(script->GetUUID());
		if (itr == scriptList.end())
		{
			return false;
		}

		Scene::getCurrentScene()->DestroyGameObject(const_cast<Script*>(script)->GetGameObject());

		scriptList.erase(itr);

		return true;
	}

	void ScriptSystem::OnGameStart()
	{
		for (auto& each : scriptList)
		{
			each.second->OnGameStart();
		}
	}

	void ScriptSystem::Clear()
	{
		for (auto& each : scriptList)
		{
			Scene::getCurrentScene()->DestroyGameObject(each.second->GetGameObject());
		}
		scriptList.clear();
	}

	bool ScriptSystem::PreEncoding(json& data) const
	{
		std::string uuidStr;
		for (auto& [uuid, ptr] : scriptList)
		{
			uuidStr = UUID_To_String(uuid);

			if (!ptr->PreEncoding(data["ScriptList"][uuidStr]["0_Pre"]))
			{
				return false;
			}
		}

		return true;
	}

	bool ScriptSystem::PostEncoding(json& data) const
	{
		std::string uuidStr;
		for (auto& [uuid, ptr] : scriptList)
		{
			uuidStr = UUID_To_String(uuid);

			auto itr = data["ScriptList"].find(uuidStr);
			if (itr == data["ScriptList"].end())
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

	bool ScriptSystem::PreDecoding(const json& data)
	{
		if (!data.contains("ScriptList"))
			return true;
		UUID uuid;
		for (auto& [uuidStr, scriptData] : data["ScriptList"].items())
		{
			uuid = String_To_UUID(uuidStr);

			auto script = CreateScript();

			if (script == nullptr)
			{
				Clear();
				return false;
			}

			scriptList.erase(script->GetUUID());

			// UUID
			script->SetUUID(uuid);
			scriptList[uuid] = script;

			if (!script->PreDecoding(scriptData["0_Pre"]))
			{
				Clear();
				return false;
			}
		}

		return true;
	}

	bool ScriptSystem::PostDecoding(const json& data)
	{
		if (!data.contains("ScriptList"))
			return true;
		UUID uuid;
		for (auto& [uuidStr, scriptData] : data["ScriptList"].items())
		{
			uuid = String_To_UUID(uuidStr);

			for (auto each : scriptList)
			{
				if (each.first == uuid)
				{
					if (!each.second->PostDecoding(scriptData["1_Post"]))
					{
						Clear();
						return false;
					}

					break;
				}
			}
		}

		return true;
	}
}
