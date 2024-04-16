/// 2024. 04. 16 김상준
/// Popup 기능을 사용할 때, 특정 시점에서 다시 Popup 을 불러들이는 등
/// Popup 관련 추가적인 기능 처리를 위한 Manager Class 입니다.

#pragma once

#include "Singleton.h"

#include "TriggerList.h"
#include "ConditionList.h"
#include "ActionList.h"

#include "imgui.h"
#include "imgui_Utility.h"

#include <stack>
#include <string>

namespace application
{
	namespace editor
	{
		class EditorPopupManager
			: public Singleton<EditorPopupManager>
		{
			friend class Singleton<EditorPopupManager>;

		public:
			void Initialize();
			void Clear();

			void SetCurrentPopup(const std::string& popupName = "");
			std::string GetCurrentPopup();
			template <typename T = void> requires std::is_base_of_v<ITrigger, T> || std::is_base_of_v<ICondition, T> || std::is_base_of_v<IAction, T> || std::is_same_v<void, T>
			void PushReturnPopup(const std::string& popupName, T* data = nullptr)
			{
				returnStack.push(popupName);
				returnDataPtr.push(data);
			}
			std::string GetReturnPopupName();
			
			template <typename T> requires std::is_base_of_v<ITrigger, T> || std::is_base_of_v<ICondition, T> || std::is_base_of_v<IAction, T> || std::is_same_v<void, T>
			T* GetReturnPopupData()
			{
				if (returnDataPtr.empty())
				{
					return nullptr;
				}

				auto ptr = returnDataPtr.top();
				return reinterpret_cast<T*>(ptr);
			}

			void Return();

		private:
			EditorPopupManager() = default;

			std::string currentPopup = "";
			std::stack<std::string> returnStack = std::stack<std::string>();
			std::stack<void*> returnDataPtr = std::stack<void*>();
		};
	}
}

