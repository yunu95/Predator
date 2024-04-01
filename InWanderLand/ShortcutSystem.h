/// 2024. 03. 27 김상준
/// Game 진행 시에 단축키 처리를 할 수 있도록 지원하는 클래스

#pragma once

#include "YunutyEngine.h"

#include <vector>
#include <unordered_map>
#include <functional>

namespace application
{
	class ShortcutSystem
		: public Component, public SingletonComponent<ShortcutSystem>
	{
		friend class SingletonComponent<ShortcutSystem>;
		friend class GameObject;

	public:
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
		/// 해당 키가 이미 Unique 키로 등록되어 있을 경우, 등록되지 않습니다.
		bool RegisterTriggerKey(unsigned int groupNum, const std::vector<std::pair<KeyCode, bool>>& keys);

		/// key 값으로 특별한 string 을 입력할 경우, 본인이 소멸에 대한 부분까지 관리를 해야합니다.
		/// Function 등록에 성공하면 true 를 반환합니다.
		bool RegisterTriggerFunction(unsigned int groupNum, const std::function<void()>& funcs, const std::string& findKey = std::string());
		
		/// key 값으로 등록했던 Function 을 지웁니다.
		/// 지우는 것에 성공하면 true 를 반환합니다.
		bool RemoveTriggerFunction(unsigned int groupNum, const std::string& findKey);

		/// 다른 기능들과 동시에 진행되지 않는 독립적인 기능을 등록하고 싶을 때, 호출하는 함수입니다.
		/// 이미 해당 키가 등록되어 있을 경우, 동작하지 않습니다.
		/// UniqueTrigger 또한 본인이 소멸에 대한 부분까지 관리해야 합니다.
		bool RegisterUniqueTrigger(const std::vector<std::pair<KeyCode, bool>>& keys, const std::function<void()>& funcs);

		/// Unique Key 값으로 등록했던 Function 을 지웁니다.
		/// 지우는 것에 성공하면 true 를 반환합니다.
		bool RemoveUniqueTrigger(const std::vector<std::pair<KeyCode, bool>>& keys);

		/// 모든 trigger switch 를 false 로 바꾸고, key 값으로 등록되지 않은 
		/// function 데이터를 삭제합니다.
		void Clear();

		/// 통일된 동작을 수행하도록 기능을 구현하고 싶을 경우 사용하는 플래그 획득 함수입니다.
		bool GetTriggerSwitch(unsigned int groupNum);

		/// 자신만의 bool 플래그를 통해서 특정 Trigger 를 유발하고 싶을 경우 사용하는 함수입니다.
		/// Shortcut System 의 TriggerSwitch 플래그를 리턴합니다.
		bool PullTrigger(unsigned int groupNum);

	private:
		ShortcutSystem();

		bool CheckUniqueKey(const std::vector<std::pair<KeyCode, bool>>& keys);
		bool CheckUniqueKeyWithinUniqueRange(const std::vector<std::pair<KeyCode, bool>>& keys);
		
		unsigned int keyCount = 72;
		std::unordered_map<unsigned int, std::set<std::pair<KeyCode, bool>, PairFirstComparator>> triggerKeys;
		std::unordered_map<unsigned int, std::vector<std::function<void()>>> triggerFunc;
		std::unordered_map<unsigned int, std::unordered_map<std::string, std::function<void()>>> managedTriggerFunc;
		std::unordered_map<unsigned int, bool> triggerSwitch;
		std::vector<std::pair<std::set<std::pair<KeyCode, bool>, PairFirstComparator>, std::function<void()>>> uniqueTrigger;
	};
}

