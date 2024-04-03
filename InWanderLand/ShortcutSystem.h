/// 2024. 03. 27 김상준
/// Game 진행 시에 단축키 처리를 할 수 있도록 지원하는 클래스

#pragma once

#include "YunutyEngine.h"

#include <vector>
#include <map>
#include <set>
#include <functional>
#include <memory>

namespace application
{
	class ShortcutSystem
		: public Component, public SingletonComponent<ShortcutSystem>
	{
		friend class SingletonComponent<ShortcutSystem>;
		friend class GameObject;

	public:
		class KeyIndex
		{
			friend class ShortcutSystem;
		public:
			KeyIndex(const KeyIndex& keyIndex) = default;
			KeyIndex& operator=(const KeyIndex& keyIndex) = default;

			auto operator<=>(const KeyIndex& keyIndex) const
			{
				if (this->keySize <=> keyIndex.keySize != 0)
				{
					return this->keySize <=> keyIndex.keySize;
				}
				else
				{
					return this->keyIndex <=> keyIndex.keyIndex;
				}
			}

			bool IsValid()
			{
				return keyIndex != MAXLONGLONG;
			}

			bool IsUnique()
			{
				return isUnique;
			}

		private:
			KeyIndex(unsigned long long keyIndex)
				: keyIndex(keyIndex)
			{
				
			}
			
			unsigned long long keyIndex = MAXLONGLONG;
			unsigned int keySize = 0;
			bool isUnique = false;
			bool isParentCalled = false;
		};

		class FuncIndex
		{
			friend class ShortcutSystem;
		public:
			FuncIndex() = default;
			FuncIndex(const FuncIndex& idx) = default;
			FuncIndex& operator=(const FuncIndex& funcIndex) = default;

			auto operator<=>(const FuncIndex& funcIndex) const
			{
				assert(this->keyIndex == funcIndex.keyIndex && "For comparison, 'keyIndex' must be the same.");
				return this->funcIndex <=> funcIndex.funcIndex;
			}

			bool IsValid()
			{
				return ((keyIndex != MAXLONGLONG) && (funcIndex != MAXLONGLONG));
			}

		private:
			FuncIndex(unsigned long long keyIndex, unsigned long long funcIndex)
				: keyIndex(keyIndex), funcIndex(funcIndex)
			{

			}

			unsigned long long keyIndex = MAXLONGLONG;
			unsigned long long funcIndex = MAXLONGLONG;
		};

		struct PairFirstComparator
		{
			bool operator()(const std::pair<KeyCode, bool>& lhs, const std::pair<KeyCode, bool>& rhs) const
			{
				return lhs.first < rhs.first;
			}
		};

		virtual void SingletonInitializer() override;
		virtual void Update() override;

		/// pair 입력값인 bool 은 true 일 경우 KeyDown 로 / false 일 경우 KeyPushed 으로 처리합니다.
		/// 등록에 성공하면 Index 를 반환합니다.
		/// 등록에 실패할 경우 assert 가 발생합니다.
		KeyIndex RegisterTriggerKey(const std::vector<std::pair<KeyCode, bool>>& keys);

		/// 특정 키의 Index 를 획득합니다.
		/// 키가 없을 경우 KeyIndex 의 IsValid 가 false 입니다.
		/// (IsUnique 인 KeyIndex 를 통해 Function 을 등록하면 assert 가 발생합니다.)
		KeyIndex GetKeyIndex(const std::vector<std::pair<KeyCode, bool>>& keys);

		/// 특정 키가 유발되었을 때 실행할 Function 을 등록합니다.
		/// 해당 함수로 등록되는 다른 Function 들이 있을 수 있음에 유의해야 합니다.
		/// (독립적인 기능 수행을 원할 경우 RegisterUniqueTrigger 를 사용하세요.)
		/// Function 등록에 성공하면 Index 를 반환합니다.
		/// 등록에 실패할 경우 assert 가 발생합니다.
		FuncIndex RegisterTriggerFunction(const KeyIndex& idx, const std::function<void()>& func);
		
		/// Index 값으로 등록했던 Function 을 지웁니다.
		/// 지우는 것에 성공하면 true 를 반환합니다.
		bool RemoveTriggerFunction(const FuncIndex& idx);

		/// 다른 기능들과 동시에 진행되지 않는 독립적인 기능을 등록하고 싶을 때, 호출하는 함수입니다.
		/// 등록에 성공하면 Index 를 반환합니다.
		/// 등록에 실패할 경우 assert 가 발생합니다.
		KeyIndex RegisterUniqueTrigger(const std::vector<std::pair<KeyCode, bool>>& keys, const std::function<void()>& func);

		/// Unique Key 값으로 등록했던 Function 을 지웁니다.
		/// 지우는 것에 성공하면 true 를 반환합니다.
		bool RemoveUniqueTrigger(const KeyIndex& idx);

		/// 모든 trigger switch 를 false 로 바꿉니다. 
		/// function 데이터의 생명은 등록한 곳에서 관리하도록 합니다.
		void Clear();

		/// 통일된 동작을 수행하도록 기능을 구현하고 싶을 경우 사용하는 플래그 획득 함수입니다.
		/// 해당 Trigger 가 없으면 assert 가 발생합니다.
		bool GetTriggerSwitch(const KeyIndex& idx);

		/// 자신만의 bool 플래그를 통해서 특정 Trigger 를 유발하고 싶을 경우 사용하는 함수입니다.
		/// 해당 Trigger 가 없으면 assert 가 발생합니다.
		/// (Unique Trigger 는 해당 함수로 유발할 수 없습니다.)
		bool PullTrigger(const KeyIndex& idx);

	private:
		ShortcutSystem();

		bool PullUniqueTrigger(const KeyIndex& idx);
		void UpdateSubIndexGroup();
		bool RemoveSubIndesGroup(const KeyIndex& idx);
		void InitSubGroup();

		unsigned long long keyIdx = 0;
		std::map<KeyIndex, std::set<std::pair<KeyCode, bool>, PairFirstComparator>, greater<KeyIndex>> triggerKeys;
		std::map<KeyIndex, std::map<FuncIndex, std::function<void()>>, greater<KeyIndex>> triggerFunc;
		std::map<KeyIndex, unsigned long long, greater<KeyIndex>> funcIndexMap;
		std::map<KeyIndex, std::pair<std::set<std::pair<KeyCode, bool>, PairFirstComparator>, std::function<void()>>, greater<KeyIndex>> uniqueTrigger;
		std::map<KeyIndex, bool, greater<KeyIndex>> triggerSwitch;
		std::map<KeyIndex, std::set<KeyIndex>, greater<KeyIndex>> subIndexGroup;
		std::set<KeyIndex, greater<KeyIndex>> allKeys;
	};
}

