/// 2024. 04. 05 김상준
/// Script System 에서 관리할 단위가 되는 Script

#pragma once

#include "Storable.h"
#include "Identifiable.h"

#include "TriggerList.h"
#include "ConditionList.h"
#include "ActionList.h"
#include "ObservationTarget.h"
#include "ProgressTracker.h"

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
		: public Identifiable, public Storable, public Component, public ObservationTarget, public ProgressTracker
	{
		friend class ScriptSystem;
		friend class editor::Module_ScriptEditor;

		friend CoroutineObject<void> Action_PullScriptTrigger::DoAction();

	public:
		virtual ~Script() = default;
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

		virtual void ProgressInitialize() override;
		virtual void CurrentProgressSave() override;
		virtual void Recovery() override;

	protected:
		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		void PushCoroutine();
		CoroutineObject<void> MakeActionCoroutine();

	private:
		/// Trigger 와 무관하게 PullTrigger 효과를 Script 단위에서 수행하는 함수입니다.
		void PullScriptTrigger();

		std::unordered_set<std::shared_ptr<ITrigger>> triggerList;
		std::unordered_set<std::shared_ptr<ICondition>> conditionList;
		std::vector<std::shared_ptr<IAction>> actionList;
		std::queue<CoroutineObject<void>> coroutineQueue = std::queue<CoroutineObject<void>>();
		std::vector<CoroutineObject<void>> coroutineInProgress = std::vector<CoroutineObject<void>>();
		bool repeat = false;
		bool doAction = false;
		bool savedAction = false;
	};

	struct ScriptPredicate
	{
		bool operator()(const Script* const ls, const Script* const rs) const
		{
			if (ls && rs)
			{
				return ls->name < rs->name;
			}

			if (!ls)
			{
				return true;
			}

			return false;
		}
	};
}

