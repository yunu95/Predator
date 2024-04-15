/// 2024. 04. 05 김상준
/// 블록 코딩 지원을 위한 Script System

#pragma once

#include "Script.h"
#include "Storable.h"

#include "YunutyEngine.h"

#include <unordered_map>
#include <unordered_set>
#include <string>

namespace application
{
	class ScriptSystem
		: public Storable, public Component, public SingletonComponent<ScriptSystem>
	{
		friend class SingletonComponent<ScriptSystem>;
		friend class GameObject;
		friend class editor::MapFileManager;

	public:
		virtual void SingletonInitializer() override;

		Script* CreateScript();
		bool EraseScript(Script* script);
		void OnGameStart();
		void OnGameStop();
		void Clear();

		std::unordered_set<Script*>& GetScriptList();

		static std::unordered_map<TriggerType, std::string> triggerList;
		static std::unordered_map<ConditionType, std::string> conditionList;
		static std::unordered_map<ActionType, std::string> actionList;

	private:
		ScriptSystem() = default;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		std::unordered_map<const UUID, Script*> scriptList;
		std::unordered_set<Script*> scriptContainer;
	};
}

