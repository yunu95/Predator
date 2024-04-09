#include "Script.h"

namespace application
{
	void Script::Update()
	{
		for (int i = 0; i < coroutineQueue.size(); i++)
		{
			auto coroutine = coroutineQueue.front();
			if (coroutine.Done())
			{
				coroutineQueue.pop();
				continue;
			}
			coroutine();
			coroutineQueue.pop();
			coroutineQueue.push(coroutine);
		}
	}

	void Script::OnGameStart()
	{
		for (auto& each : triggerList)
		{
			each->Clear();
			each->AddFunction([=]()
				{
					bool isConditionMet = true;
					for (auto& each : conditionList)
					{
						isConditionMet &= each->IsConditionMet();
					}

					if (isConditionMet)
					{
						PushCoroutine();
					}
				});
			each->LinkCallback();
		}
	}

	bool Script::EraseTrigger(const std::shared_ptr<ITrigger>& ptr)
	{
		if (triggerList.find(ptr) == triggerList.end())
		{
			return false;
		}

		triggerList.erase(ptr);

		return true;
	}

	bool Script::EraseCondition(const std::shared_ptr<ICondition>& ptr)
	{
		if (conditionList.find(ptr) == conditionList.end())
		{
			return false;
		}

		conditionList.erase(ptr);

		return true;
	}

	bool Script::EraseAction(const std::shared_ptr<IAction>& ptr)
	{
		if (actionList.find(ptr) == actionList.end())
		{
			return false;
		}

		actionList.erase(ptr);

		return true;
	}

	bool Script::PreEncoding(json& data) const
	{
		return true;
	}

	bool Script::PostEncoding(json& data) const
	{
		return true;
	}

	bool Script::PreDecoding(const json& data)
	{
		return true;
	}

	bool Script::PostDecoding(const json& data)
	{
		return true;
	}

	void Script::PushCoroutine()
	{
		coroutineQueue.push(MakeActionCoroutine());
	}

	CoroutineObject<void> Script::MakeActionCoroutine()
	{
		for (auto& each : actionList)
		{
			auto coroutine = each->DoAction();
			while (!coroutine.Done())
			{
				coroutine();
				co_await std::suspend_always();
			}
		}
	}
}
