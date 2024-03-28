/// 2024. 03. 27 김상준
/// Game 진행 시에 단축키 처리를 할 수 있도록 지원하는 클래스

#pragma once

#include "YunutyEngine.h"

#include <vector>
#include <unordered_set>
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
		virtual void SingletonInitializer() override;
		virtual void Update() override;

		/// pair 입력값인 bool 은 true 일 경우 KeyDown 로 / false 일 경우 KeyPushed 으로 처리합니다.
		bool RegisterTriggerKey(unsigned int groupNum, const std::vector<std::pair<KeyCode, bool>>& keys);
		bool RegisterTriggerFunction(unsigned int groupNum, const std::function<void(GameObject*)>& funcs);
		bool RegisterObject(unsigned int groupNum, GameObject* obj);
		void ClearObject();

	private:
		ShortcutSystem();
		
		unsigned int keyCount = 72;
		std::vector<std::unordered_set<GameObject*>> list;
		std::unordered_map<unsigned int, std::vector<std::pair<KeyCode, bool>>> triggerKeys;
		std::unordered_map<unsigned int, std::function<void(GameObject*)>> triggerFunc;
	};
}

