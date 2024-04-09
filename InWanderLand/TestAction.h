#pragma once

#include "IAction.h"

#include "Action_WaitForSeconds.h"

#include <iostream>

namespace application
{
	class TestAction
		: public IAction
	{
	public:
		DEFINE_ACTION(TestAction)

		virtual CoroutineObject<void> DoAction() override
		{
			auto coroutine = Action_WaitForSeconds(4).DoAction();
			for (const auto& val : coroutine)
			{
				co_await std::suspend_always();
			}

			std::cout << "Hello! Coroutine!! 4sec" << std::endl;

			auto coroutine2 = Action_WaitForSeconds(2).DoAction();
			for (const auto& val : coroutine2)
			{
				co_await std::suspend_always();
			}

			std::cout << "Hello! Coroutine!! 2sec" << std::endl;
		}

		virtual bool PreEncoding(json& data) const override
		{
			return true;
		}

		virtual bool PostEncoding(json& data) const override
		{
			return true;
		}

		virtual bool PreDecoding(const json& data) override
		{
			return true;
		}

		virtual bool PostDecoding(const json& data) override
		{
			return true;
		}

		TestAction() = default;
	};
}
