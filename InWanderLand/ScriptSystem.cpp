#include "ScriptSystem.h"

#include "Script.h"
#include "YunutyEngine.h"

namespace application
{
	std::unordered_map<TriggerType, std::string> ScriptSystem::triggerList = std::unordered_map<TriggerType, std::string>();
	std::unordered_map<ConditionType, std::string> ScriptSystem::conditionList = std::unordered_map<ConditionType, std::string>();
	std::unordered_map<ActionType, std::string> ScriptSystem::actionList = std::unordered_map<ActionType, std::string>();

	void ScriptSystem::SingletonInitializer()
	{
		/// Trigger
		triggerList[TriggerType::GameStart] = "GameStart";
		triggerList[TriggerType::EnterRegion] = "EnterRegion";
		triggerList[TriggerType::LeaveRegion] = "LeaveRegion";
		triggerList[TriggerType::RepeatPeriodically] = "RepeatPeriodically";
		triggerList[TriggerType::RepeatPeriodicallyRealTime] = "RepeatPeriodicallyRealTime";
		triggerList[TriggerType::UnitAppear] = "UnitAppear";
		triggerList[TriggerType::UnitDie] = "UnitDie";

		/// Condition
		conditionList[ConditionType::CinematicModeOn] = "CinematicModeOn";
		conditionList[ConditionType::CinematicModeOff] = "CinematicModeOff";

		/// Action
		actionList[ActionType::WaitForSeconds] = "WaitForSeconds";
		actionList[ActionType::WaitForRealSeconds] = "WaitForRealSeconds";
		actionList[ActionType::CinematicModeChange] = "CinematicModeChange";
		actionList[ActionType::CameraChangeView] = "CameraChangeView";
		actionList[ActionType::CameraSaveView] = "CameraSaveView";
		actionList[ActionType::CameraLoadView] = "CameraLoadView";
	}

	Script* ScriptSystem::CreateScript()
	{
		auto sptr = Scene::getCurrentScene()->AddGameObject()->AddComponent<Script>();
		scriptList[sptr->GetUUID()] = sptr;
		scriptContainer.insert(sptr);
		return sptr;
	}

	bool ScriptSystem::EraseScript(Script* script)
	{
		auto itr = scriptList.find(script->GetUUID());
		if (itr == scriptList.end())
		{
			return false;
		}

		Scene::getCurrentScene()->DestroyGameObject(script->GetGameObject());

		scriptList.erase(itr);
		scriptContainer.erase(script);

		return true;
	}

	void ScriptSystem::PreMapLoad()
	{
		Action_CameraSaveView::Clear();

		Clear();
	}

	void ScriptSystem::OnGameStart()
	{
		Action_CameraSaveView::Clear();

		for (auto& each : scriptList)
		{
			each.second->OnGameStart();
		}
	}

	void ScriptSystem::OnGameStop()
	{
		Action_CameraSaveView::Clear();

		for (auto& each : scriptList)
		{
			each.second->OnGameStop();
		}
	}

	std::unordered_set<Script*>& ScriptSystem::GetScriptList()
	{
		return scriptContainer;
	}

	void ScriptSystem::Clear()
	{
		for (auto& each : scriptList)
		{
			Scene::getCurrentScene()->DestroyGameObject(each.second->GetGameObject());
		}
		scriptList.clear();
		scriptContainer.clear();
	}

	bool ScriptSystem::PreEncoding(json& data) const
	{
		std::string uuidStr;
		for (auto& [uuid, ptr] : scriptList)
		{
			uuidStr = UUID_To_String(uuid);

			if (!ptr->PreEncoding(data["ScriptList"][uuidStr]))
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

			if (!ptr->PostEncoding(itr.value()))
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

			if (!script->PreDecoding(scriptData))
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

			for (auto& each : scriptList)
			{
				if (each.first == uuid)
				{
					if (!each.second->PostDecoding(scriptData))
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
