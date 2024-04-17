/// 2024. 04. 05 김상준
/// Script System 에서 관리할 단위가 되는 Script

#pragma once

#include "Storable.h"
#include "Identifiable.h"

#include "TriggerList.h"
#include "ConditionList.h"
#include "ActionList.h"

#include "YunutyEngine.h"

#include <type_traits>
#include <vector>
#include <unordered_set>
#include <memory>
#include <queue>

namespace application
{
	namespace editor
	{
		class Module_ScriptEditor;
	}
}

namespace application
{
	class Script
		: public Identifiable, public Storable, public Component
	{
		friend class ScriptSystem;
		friend class editor::Module_ScriptEditor;

	public:
		virtual void Update() override;

		std::string name = "NewScript";

		void OnGameStart();
		void OnGameStop();

		template <typename T, typename... Types> requires std::is_base_of_v<ITrigger, T>
		std::shared_ptr<ITrigger> AddTrigger(Types... args)
		{
			auto sptr = std::make_shared<T>(args...);
			triggerList.insert(sptr);
			return sptr;
		}

		bool EraseTrigger(const std::shared_ptr<ITrigger>& ptr);

		template <typename T, typename... Types> requires std::is_base_of_v<ICondition, T>
		std::shared_ptr<ICondition> AddCondition(Types... args)
		{
			auto sptr = std::make_shared<T>(args...);
			conditionList.insert(sptr);
			return sptr;
		}

		bool EraseCondition(const std::shared_ptr<ICondition>& ptr);

		template <typename T, typename... Types> requires std::is_base_of_v<IAction, T>
		std::shared_ptr<IAction> AddAction(Types... args)
		{
			auto sptr = std::make_shared<T>(args...);
			actionList.push_back(sptr);
			return sptr;
		}

		bool EraseAction(const std::shared_ptr<IAction>& ptr);

		bool ReorderAction(const std::shared_ptr<IAction>& ptr, unsigned int idx);

	protected:
		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		void PushCoroutine();
		CoroutineObject<void> MakeActionCoroutine();

	private:
		std::unordered_set<std::shared_ptr<ITrigger>> triggerList;
		std::unordered_set<std::shared_ptr<ICondition>> conditionList;
		std::vector<std::shared_ptr<IAction>> actionList;
		std::queue<CoroutineObject<void>> coroutineQueue = std::queue<CoroutineObject<void>>();
		std::vector<CoroutineObject<void>> coroutineInProgress = std::vector<CoroutineObject<void>>();
	};
}

