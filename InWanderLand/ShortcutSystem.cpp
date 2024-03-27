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