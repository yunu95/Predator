#include "ShortcutSystem.h"
#include "DebugGraphic.h"

#include "YunutyEngine.h"

namespace application
{
	void ShortcutSystem::SingletonInitializer()
	{
		triggerKeys.reserve(keyCount);
		triggerFunc.reserve(keyCount);
		managedTriggerFunc.reserve(keyCount);
		triggerSwitch.reserve(keyCount);

		/// 콘텐츠 영역에서 세팅할 수 있도록 public 으로 함수를 열었으나,
		/// 우선 여기에서 일괄 처리하는 방식으로 구현함
		std::vector<std::pair<KeyCode, bool>> keys;
		keys.push_back({ KeyCode::Control, true });
		keys.push_back({ KeyCode::NUM_1, false });
		RegisterTriggerKey(1, keys);
		keys[1] = { KeyCode::NUM_2, false };
		RegisterTriggerKey(2, keys);
		keys[1] = { KeyCode::NUM_3, false };
		RegisterTriggerKey(3, keys);
		keys[1] = { KeyCode::NUM_4, false };
		RegisterTriggerKey(4, keys);
		keys[1] = { KeyCode::NUM_5, false };
		RegisterTriggerKey(5, keys);
		keys[1] = { KeyCode::NUM_6, false };
		RegisterTriggerKey(6, keys);
		keys[1] = { KeyCode::NUM_7, false };
		RegisterTriggerKey(7, keys);
		keys[1] = { KeyCode::NUM_8, false };
		RegisterTriggerKey(8, keys);
		keys[1] = { KeyCode::NUM_9, false };
		RegisterTriggerKey(9, keys);
		keys[1] = { KeyCode::NUM_0, false };
		RegisterTriggerKey(0, keys);
		keys[1] = { KeyCode::Q, false };
		RegisterTriggerKey(10, keys);
		keys[1] = { KeyCode::W, false };
		RegisterTriggerKey(11, keys);
		keys[1] = { KeyCode::Alphabet_E, false };
		RegisterTriggerKey(12, keys);
		keys[1] = { KeyCode::R, false };
		RegisterTriggerKey(13, keys);
		keys[1] = { KeyCode::T, false };
		RegisterTriggerKey(14, keys);
		keys[1] = { KeyCode::Y, false };
		RegisterTriggerKey(15, keys);
		keys[1] = { KeyCode::U, false };
		RegisterTriggerKey(16, keys);
		keys[1] = { KeyCode::I, false };
		RegisterTriggerKey(17, keys);
		keys[1] = { KeyCode::O, false };
		RegisterTriggerKey(18, keys);
		keys[1] = { KeyCode::P, false };
		RegisterTriggerKey(19, keys);
		keys[1] = { KeyCode::A, false };
		RegisterTriggerKey(20, keys);
		keys[1] = { KeyCode::S, false };
		RegisterTriggerKey(21, keys);
		keys[1] = { KeyCode::D, false };
		RegisterTriggerKey(22, keys);
		keys[1] = { KeyCode::F, false };
		RegisterTriggerKey(23, keys);
		keys[1] = { KeyCode::G, false };
		RegisterTriggerKey(24, keys);
		keys[1] = { KeyCode::H, false };
		RegisterTriggerKey(25, keys);
		keys[1] = { KeyCode::J, false };
		RegisterTriggerKey(26, keys);
		keys[1] = { KeyCode::K, false };
		RegisterTriggerKey(27, keys);
		keys[1] = { KeyCode::L, false };
		RegisterTriggerKey(28, keys);
		keys[1] = { KeyCode::Z, false };
		RegisterTriggerKey(29, keys);
		keys[1] = { KeyCode::X, false };
		RegisterTriggerKey(30, keys);
		keys[1] = { KeyCode::C, false };
		RegisterTriggerKey(31, keys);
		keys[1] = { KeyCode::V, false };
		RegisterTriggerKey(32, keys);
		keys[1] = { KeyCode::B, false };
		RegisterTriggerKey(33, keys);
		keys[1] = { KeyCode::N, false };
		RegisterTriggerKey(34, keys);
		keys[1] = { KeyCode::M, false };
		RegisterTriggerKey(35, keys);

		keys[0] = { KeyCode::LShift, true };
		keys[1] = { KeyCode::NUM_1, false };
		RegisterTriggerKey(36, keys);
		keys[1] = { KeyCode::NUM_2, false };
		RegisterTriggerKey(37, keys);
		keys[1] = { KeyCode::NUM_3, false };
		RegisterTriggerKey(38, keys);
		keys[1] = { KeyCode::NUM_4, false };
		RegisterTriggerKey(39, keys);
		keys[1] = { KeyCode::NUM_5, false };
		RegisterTriggerKey(40, keys);
		keys[1] = { KeyCode::NUM_6, false };
		RegisterTriggerKey(41, keys);
		keys[1] = { KeyCode::NUM_7, false };
		RegisterTriggerKey(42, keys);
		keys[1] = { KeyCode::NUM_8, false };
		RegisterTriggerKey(43, keys);
		keys[1] = { KeyCode::NUM_9, false };
		RegisterTriggerKey(44, keys);
		keys[1] = { KeyCode::NUM_0, false };
		RegisterTriggerKey(45, keys);
		keys[1] = { KeyCode::Q, false };
		RegisterTriggerKey(46, keys);
		keys[1] = { KeyCode::W, false };
		RegisterTriggerKey(47, keys);
		keys[1] = { KeyCode::Alphabet_E, false };
		RegisterTriggerKey(48, keys);
		keys[1] = { KeyCode::R, false };
		RegisterTriggerKey(49, keys);
		keys[1] = { KeyCode::T, false };
		RegisterTriggerKey(50, keys);
		keys[1] = { KeyCode::Y, false };
		RegisterTriggerKey(51, keys);
		keys[1] = { KeyCode::U, false };
		RegisterTriggerKey(52, keys);
		keys[1] = { KeyCode::I, false };
		RegisterTriggerKey(53, keys);
		keys[1] = { KeyCode::O, false };
		RegisterTriggerKey(54, keys);
		keys[1] = { KeyCode::P, false };
		RegisterTriggerKey(55, keys);
		keys[1] = { KeyCode::A, false };
		RegisterTriggerKey(56, keys);
		keys[1] = { KeyCode::S, false };
		RegisterTriggerKey(57, keys);
		keys[1] = { KeyCode::D, false };
		RegisterTriggerKey(58, keys);
		keys[1] = { KeyCode::F, false };
		RegisterTriggerKey(59, keys);
		keys[1] = { KeyCode::G, false };
		RegisterTriggerKey(60, keys);
		keys[1] = { KeyCode::H, false };
		RegisterTriggerKey(61, keys);
		keys[1] = { KeyCode::J, false };
		RegisterTriggerKey(62, keys);
		keys[1] = { KeyCode::K, false };
		RegisterTriggerKey(63, keys);
		keys[1] = { KeyCode::L, false };
		RegisterTriggerKey(64, keys);
		keys[1] = { KeyCode::Z, false };
		RegisterTriggerKey(65, keys);
		keys[1] = { KeyCode::X, false };
		RegisterTriggerKey(66, keys);
		keys[1] = { KeyCode::C, false };
		RegisterTriggerKey(67, keys);
		keys[1] = { KeyCode::V, false };
		RegisterTriggerKey(68, keys);
		keys[1] = { KeyCode::B, false };
		RegisterTriggerKey(69, keys);
		keys[1] = { KeyCode::N, false };
		RegisterTriggerKey(70, keys);
		keys[1] = { KeyCode::M, false };
		RegisterTriggerKey(71, keys);
	}

	void ShortcutSystem::Update()
	{
		for (int i = 0; i < keyCount; i++)
		{
			bool trigger = true;
			for (auto& [key, flag] : triggerKeys[i])
			{
				if (flag)
				{
					trigger &= Input::isKeyDown(key);
				}
				else
				{
					trigger &= Input::isKeyPushed(key);
				}
			}

			if (trigger)
			{
				PullTrigger(i);
			}
		}
	}

	bool ShortcutSystem::RegisterTriggerKey(unsigned int groupNum, const std::vector<std::pair<KeyCode, bool>>& keys)
	{
		if (groupNum >= keyCount || triggerKeys.find(groupNum) != triggerKeys.end())
		{
			return false;
		}

		triggerKeys[groupNum] = keys;

		return true;
	}

	bool ShortcutSystem::RegisterTriggerFunction(unsigned int groupNum, const std::function<void()>& funcs, const std::string& findKey)
	{
		if (groupNum >= keyCount)
		{
			return false;
		}

		if (findKey.empty())
		{
			triggerFunc[groupNum].push_back(funcs);
		}
		else
		{
			if (managedTriggerFunc[groupNum].find(findKey) != managedTriggerFunc[groupNum].end())
			{
				return false;
			}

			managedTriggerFunc[groupNum][findKey] = funcs;
		}

		return true;
	}

	bool ShortcutSystem::RemoveTriggerFunction(unsigned int groupNum, const std::string& findKey)
	{
		if (groupNum >= keyCount || managedTriggerFunc.find(groupNum) != managedTriggerFunc.end())
		{
			return false;
		}

		for (auto& [key, func] : managedTriggerFunc[groupNum])
		{
			if (key == findKey)
			{
				managedTriggerFunc[groupNum].erase(findKey);
				return true;
			}
		}

		return false;
	}

	void ShortcutSystem::Clear()
	{
		triggerFunc.clear();
		triggerFunc.reserve(keyCount);

		for (auto& [key, val] : triggerSwitch)
		{
			val = false;
		}
	}

	bool ShortcutSystem::GetTriggerSwitch(unsigned int groupNum)
	{
		if (groupNum >= keyCount)
		{
			return false;
		}

		return triggerSwitch[groupNum];
	}

	bool ShortcutSystem::PullTrigger(unsigned int groupNum)
	{
		if (groupNum >= keyCount)
		{
			return false;
		}

		triggerSwitch[groupNum] = !triggerSwitch[groupNum];

		if (triggerFunc.find(groupNum) != triggerFunc.end())
		{
			for (auto& each : triggerFunc[groupNum])
			{
				each();
			}
		}

		if (managedTriggerFunc.find(groupNum) != managedTriggerFunc.end())
		{
			for (auto& each : managedTriggerFunc[groupNum])
			{
				each.second();
			}
		}

		return triggerSwitch[groupNum];
	}

	ShortcutSystem::ShortcutSystem()
		: triggerKeys(), triggerFunc(), managedTriggerFunc(), triggerSwitch()
	{

	}
}