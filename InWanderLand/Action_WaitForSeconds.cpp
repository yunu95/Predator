#include "Action_WaitForSeconds.h"

#include "YunutyEngine.h"

namespace application
{
	CoroutineObject<void> Action_WaitForSeconds::DoAction()
	{
		for (float deltaTime = yunutyEngine::Time::GetDeltaTime(); deltaTime < waitTime;)
		{
			co_await std::suspend_always();
			deltaTime += yunutyEngine::Time::GetDeltaTime();
		}

		co_return;
	}

	bool Action_WaitForSeconds::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_WaitForSeconds::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_WaitForSeconds::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_WaitForSeconds::PostDecoding(const json& data)
	{
		return true;
	}

	Action_WaitForSeconds::Action_WaitForSeconds(const float& waitTime)
		: waitTime(waitTime)
	{

	}

	CoroutineObject<void> Action_WaitForRealSeconds::DoAction()
	{
		for (float deltaTime = yunutyEngine::Time::GetDeltaTimeUnscaled(); deltaTime < waitTime;)
		{
			co_await std::suspend_always();
			deltaTime += yunutyEngine::Time::GetDeltaTimeUnscaled();
		}

		co_return;
	}

	bool Action_WaitForRealSeconds::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_WaitForRealSeconds::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_WaitForRealSeconds::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_WaitForRealSeconds::PostDecoding(const json& data)
	{
		return true;
	}

	Action_WaitForRealSeconds::Action_WaitForRealSeconds(const float& waitTime)
		: waitTime(waitTime)
	{

	}
}