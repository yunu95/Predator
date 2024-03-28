#include "ShortcutSystem.h"

#include "YunutyEngine.h"

namespace application
{
	void ShortcutSystem::SingletonInitializer()
	{
		list.resize(keyCount);
		triggerKeys.reserve(keyCount);
		triggerFunc.reserve(keyCount);

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

		for (int i = 0; i < keyCount; i++)
		{
			triggerFunc[i] = [&](GameObject* obj)
				{
					obj->SetSelfActive(!obj->GetActive());
				};
		}
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
				for (auto& each : list[i])
				{
					triggerFunc[i](each);
				}
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

	bool ShortcutSystem::RegisterTriggerFunction(unsigned int groupNum, const std::function<void(GameObject*)>& funcs)
	{
		if (groupNum >= keyCount || triggerFunc.find(groupNum) != triggerFunc.end())
		{
			return false;
		}

		triggerFunc[groupNum] = funcs;

		return true;
	}

	bool ShortcutSystem::RegisterObject(unsigned int groupNum, GameObject* obj)
	{
		if (groupNum >= keyCount)
		{
			return false;
		}

		list[groupNum].insert(obj);

		return false;
	}

	void ShortcutSystem::ClearObject()
	{
		/// Object 가 게임 중에서 소멸되어 잘못된 메모리에 접근할 가능성이 있음
		/// 우선, 게임 진행 중에서는 그러한 경우가 없다고 가정하고
		/// 플레이 전/후 에 RegisterObject 와 ClearObject 로 처리하는 방식을 사용함

		list.clear();
		list.resize(keyCount);
	}

	ShortcutSystem::ShortcutSystem()
		: list(), triggerKeys(), triggerFunc()
	{

	}
}