#pragma once

#include "IAction.h"

#include "Action_WaitForSeconds.h"

#include <iostream>

namespace application
{
	class Action_Example
		: public IAction
	{
	public:
		DEFINE_ACTION(Example)

		virtual CoroutineObject<void> DoAction() override
		{
			auto coroutine = Action_WaitForSeconds(4).DoAction();
			for (const auto& val : coroutine)
			{
				co_await std::suspend_always();
			}

			std::cout << "Hello! Coroutine!! Scaled 4sec" << std::endl;

			auto coroutine2 = Action_WaitForRealSeconds(2).DoAction();
			for (const auto& val : coroutine2)
			{
				co_await std::suspend_always();
			}

			std::cout << "Hello! Coroutine!! Real 2sec" << std::endl;
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

		Action_Example() = default;
	};

	void Action_Example::ImGui_DrawDataPopup(Action_Example* data)
	{
		
	}
}
