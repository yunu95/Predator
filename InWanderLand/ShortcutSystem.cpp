#include "ShortcutSystem.h"
#include "DebugGraphic.h"

#include "YunutyEngine.h"

namespace application
{
	void ShortcutSystem::SingletonInitializer()
	{

	}

	void ShortcutSystem::Update()
	{
		InitSubGroup();

		for (auto& idx : allKeys)
		{
			if (idx.isUnique)
			{
				bool trigger = true;
				for (auto& [keyCode, flag] : uniqueTrigger[idx].first)
				{
					if (flag)
					{
						trigger &= Input::isKeyDown(keyCode);
					}
					else
					{
						trigger &= Input::isKeyPushed(keyCode);
					}
				}

				if (trigger && (uniqueTrigger[idx].first.size() != 0))
				{
					PullUniqueTrigger(idx);
				}
			}
			else
			{
				bool trigger = true;
				for (auto& [keyCode, flag] : triggerKeys[idx])
				{
					if (flag)
					{
						trigger &= Input::isKeyDown(keyCode);
					}
					else
					{
						trigger &= Input::isKeyPushed(keyCode);
					}
				}

				if (trigger && (triggerKeys[idx].size() != 0))
				{
					PullTrigger(idx);
				}
			}
		}
	}

	ShortcutSystem::KeyIndex ShortcutSystem::RegisterTriggerKey(const std::vector<std::pair<KeyCode, bool>>& keys)
	{
		assert(keys.size() != 0 && "Key is empty.");
		assert(!GetKeyIndex(keys).IsValid() && "There is already a registered key.");

		auto returnVal = KeyIndex(keyIdx);
		returnVal.keySize = keys.size();
		keyIdx++;

		for (auto& each : keys)
		{
			triggerKeys[returnVal].insert(each);
		}

		triggerSwitch[returnVal] = false;
		allKeys.insert(returnVal);

		UpdateSubIndexGroup();

		return returnVal;
	}

	ShortcutSystem::KeyIndex ShortcutSystem::GetKeyIndex(const std::vector<std::pair<KeyCode, bool>>& keys)
	{
		for (auto& [idx, triggerKey] : triggerKeys)
		{
			if (keys.size() != triggerKey.size())
			{
				continue;
			}

			std::set<std::pair<KeyCode, bool>, PairFirstComparator> sortedList;
			for (auto& each : keys)
			{
				sortedList.insert(each);
			}

			auto itr1 = sortedList.begin();
			auto itr2 = triggerKey.begin();

			bool isSame = true;
			for (int i = 0; i < sortedList.size(); i++)
			{
				if (itr1->first != itr2->first)
				{
					isSame = false;
					break;
				}
				itr1 = ++itr1;
				itr2 = ++itr2;
			}

			if (isSame)
			{
				return idx;
			}
		}

		for (auto& [idx, pair] : uniqueTrigger)
		{
			if (keys.size() != pair.first.size())
			{
				continue;
			}

			std::set<std::pair<KeyCode, bool>, PairFirstComparator> sortedList;
			for (auto& each : keys)
			{
				sortedList.insert(each);
			}

			auto itr1 = sortedList.begin();
			auto itr2 = pair.first.begin();

			bool isSame = true;
			for (int i = 0; i < sortedList.size(); i++)
			{
				if (itr1->first != itr2->first)
				{
					isSame = false;
					break;
				}
				itr1 = ++itr1;
				itr2 = ++itr2;
			}

			if (isSame)
			{
				return idx;
			}
		}

		return KeyIndex(MAXLONGLONG);
	}

	ShortcutSystem::FuncIndex ShortcutSystem::RegisterTriggerFunction(const KeyIndex& idx, const std::function<void()>& func)
	{
		assert(!idx.isUnique && "You cannot register with Unique Key.");
		unsigned int funcIdx = 0;
		if (funcIndexMap.find(idx) == funcIndexMap.end())
		{
			funcIndexMap[idx] = 0;
		}
		else
		{
			funcIdx = funcIndexMap[idx];
		}

		auto returnVal = FuncIndex(idx.keyIndex, funcIdx);

		triggerFunc[idx].insert({ returnVal, func });

		funcIndexMap[idx] += 1;

		return returnVal;
	}

	bool ShortcutSystem::RemoveTriggerFunction(const FuncIndex& idx)
	{
		auto itr = triggerFunc.find(idx.keyIndex);
		if (itr == triggerFunc.end())
		{
			return false;
		}

		auto funcItr = itr->second.find(idx);
		if (funcItr == itr->second.end())
		{
			return false;
		}

		itr->second.erase(idx);
		return true;
	}

	ShortcutSystem::KeyIndex ShortcutSystem::RegisterUniqueTrigger(const std::vector<std::pair<KeyCode, bool>>& keys, const std::function<void()>& func)
	{
		assert(keys.size() != 0 && "Key is empty.");
		assert(!GetKeyIndex(keys).IsValid() && "There is already a registered key.");

		auto returnVal = KeyIndex(keyIdx);
		returnVal.isUnique = true;
		returnVal.keySize = keys.size();
		keyIdx++;

		std::set<std::pair<KeyCode, bool>, PairFirstComparator> keyList;
		for (auto& each : keys)
		{
			keyList.insert(each);
		}

		uniqueTrigger.insert({ returnVal, {keyList, func} });

		triggerSwitch[returnVal] = false;
		allKeys.insert(returnVal);

		UpdateSubIndexGroup();

		return returnVal;
	}

	bool ShortcutSystem::RemoveUniqueTrigger(const KeyIndex& idx)
	{
		if (!idx.isUnique)
		{
			return false;
		}

		auto itr = uniqueTrigger.find(idx);
		if (itr == uniqueTrigger.end())
		{
			return false;
		}

		uniqueTrigger.erase(idx);
		allKeys.erase(idx);
		triggerSwitch.erase(idx);
		subIndexGroup.erase(idx);
		return true;
	}

	void ShortcutSystem::Clear()
	{
		for (auto& [key, val] : triggerSwitch)
		{
			val = false;
		}
	}

	bool ShortcutSystem::GetTriggerSwitch(const KeyIndex& idx)
	{
		auto itr = triggerSwitch.find(idx);
		assert(itr != triggerSwitch.end() && "There is no corresponding trigger.");

		return itr->second;
	}

	bool ShortcutSystem::PullTrigger(const KeyIndex& idx)
	{
		auto itr = triggerKeys.find(idx);
		assert(itr != triggerKeys.end() && "There is no corresponding trigger.");

		if (idx.isParentCalled)
		{
			return false;
		}

		for (auto& each : triggerFunc[idx])
		{
			each.second();
		}

		triggerSwitch[idx] = !triggerSwitch[idx];

		auto subItr = subIndexGroup.find(idx);
		if (subItr != subIndexGroup.end())
		{
			for (auto& each : subIndexGroup[idx])
			{
				const_cast<KeyIndex&>(*allKeys.find(each)).isParentCalled = true;
			}
		}

		return true;
	}

	ShortcutSystem::ShortcutSystem()
		: triggerKeys(), triggerFunc(), funcIndexMap(), uniqueTrigger(), triggerSwitch(), subIndexGroup(), allKeys()
	{

	}

	bool ShortcutSystem::PullUniqueTrigger(const KeyIndex& idx)
	{
		assert(idx.isUnique && "Generic triggers can not be triggered by this function.");

		auto itr = uniqueTrigger.find(idx);
		assert(itr != uniqueTrigger.end() && "There is no corresponding unique trigger.");

		if (idx.isParentCalled)
		{
			return false;
		}

		uniqueTrigger[idx].second();
		triggerSwitch[idx] = !triggerSwitch[idx];

		auto subItr = subIndexGroup.find(idx);
		if (subItr != subIndexGroup.end())
		{
			for (auto& each : subIndexGroup[idx])
			{
				const_cast<KeyIndex&>(*allKeys.find(each)).isParentCalled = true;
			}
		}

		return true;
	}

	void ShortcutSystem::UpdateSubIndexGroup()
	{
		for (auto& each : allKeys)
		{
			for (auto& [compIdx, keyCodes] : triggerKeys)
			{
				bool subIndex = true;
				if (compIdx.keySize >= each.keySize)
				{
					continue;
				}

				for (auto& [keyCode, flag] : keyCodes)
				{
					bool exist = false;
					if (each.isUnique)
					{
						for (auto& keyPair : uniqueTrigger[each].first)
						{
							if (keyPair.first == keyCode)
							{
								exist = true;
								break;
							}
						}
					}
					else
					{
						for (auto& keyPair : triggerKeys[each])
						{
							if (keyPair.first == keyCode)
							{
								exist = true;
								break;
							}
						}
					}

					if (!exist)
					{
						subIndex = false;
						break;
					}
				}

				if (subIndex)
				{
					subIndexGroup[each].insert(compIdx);
				}
			}

			for (auto& [compIdx, compPair] : uniqueTrigger)
			{
				bool subIndex = true;
				if (compIdx.keySize >= each.keySize)
				{
					continue;
				}

				for (auto& [keyCode, flag] : compPair.first)
				{
					bool exist = false;
					if (each.isUnique)
					{
						for (auto& keyPair : uniqueTrigger[each].first)
						{
							if (keyPair.first == keyCode)
							{
								exist = true;
								break;
							}
						}
					}
					else
					{
						for (auto& keyPair : triggerKeys[each])
						{
							if (keyPair.first == keyCode)
							{
								exist = true;
								break;
							}
						}
					}

					if (!exist)
					{
						subIndex = false;
						break;
					}
				}

				if (subIndex)
				{
					subIndexGroup[each].insert(compIdx);
				}
			}
		}
	}

	bool ShortcutSystem::RemoveSubIndesGroup(const KeyIndex& idx)
	{
		auto itr = subIndexGroup.find(idx);
		if (itr != subIndexGroup.end())
		{
			subIndexGroup.erase(idx);
			return true;
		}

		return false;
	}

	void ShortcutSystem::InitSubGroup()
	{
		for (auto& each : allKeys)
		{
			const_cast<KeyIndex&>(each).isParentCalled = false;
		}
	}
}