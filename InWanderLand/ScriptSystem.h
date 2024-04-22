/// 2024. 04. 05 김상준
/// 블록 코딩 지원을 위한 Script System

#pragma once

#include "Storable.h"

#include "ITrigger.h"
#include "ICondition.h"
#include "IAction.h"

#include "YunutyEngine.h"

#include "PlayableComponent.h"

#include <unordered_map>
#include <unordered_set>
#include <string>

namespace application
{
	class Script;

	namespace editor
	{
		class MapFileManager;
	}
}

namespace application
{
	class ScriptSystem
		: public Storable, public PlayableComponent, public Component, public SingletonComponent<ScriptSystem>
	{
		friend class SingletonComponent<ScriptSystem>;
		friend class GameObject;
		friend class editor::MapFileManager;

	public:
		virtual void SingletonInitializer() override;

		Script* CreateScript();
		bool EraseScript(Script* script);

		virtual void PreMapLoad() override;
		virtual void OnGameStart() override;
		virtual void OnGameStop() override;

		std::unordered_set<Script*>& GetScriptList();

		static std::vector<std::pair<TriggerType, std::string>> triggerList;
		static std::vector<std::pair<ConditionType, std::string>> conditionList;
		static std::vector<std::pair<ActionType, std::string>> actionList;

	private:
		ScriptSystem() = default;

		void Clear();

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		std::unordered_map<const UUID, Script*> scriptList;
		std::unordered_set<Script*> scriptContainer;
	};
}

