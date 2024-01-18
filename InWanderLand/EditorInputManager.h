/// 2024. 01. 10 김상준
/// 에디터의 입력 처리를 위한 InputManager

#pragma once

#ifdef EDITOR
#include "Singleton.h"
#include "EditorKeyCodes.h"

#include <map>
#include <unordered_map>

namespace application
{
	namespace editor
	{
		enum class KeyState
		{
			None = 0,
			Pressed,    // 처음 눌러짐
			Down,       // 계속 누르고 있는 상태
			Up          // 떼어냄
		};

		struct KeyboardData
		{
			KeyCode keyCode = KeyCode::NoKey;
			KeyState old = KeyState::None;
			KeyState now = KeyState::None;
		};

		struct MouseData
		{
			MouseCode mouseCode = MouseCode::None;
			KeyState old = KeyState::None;
			KeyState now = KeyState::None;
		};

		class EditorInputManager
			: public Singleton<EditorInputManager>
		{
			friend class Singleton<EditorInputManager>;

		public:
			static KeyCode GetKeycode(const int& key);

			void Update();

			bool IsKeyboardPressed(KeyCode keyCode) const;
			bool IsKeyboardDown(KeyCode keyCode) const;
			bool IsKeyboardUp(KeyCode keyCode) const;

			bool IsMouseButtonPressed(MouseCode mouseCode) const;
			bool IsMouseButtonDown(MouseCode mouseCode) const;
			bool IsMouseButtonUp(MouseCode mouseCode) const;

			void UpdateKeyboardState(KeyCode keyCode, KeyState newState);
			void UpdateMouseButtonState(MouseCode mouseCode, KeyState newState);
			// Up 상태를 모두 None 으로 처리
			void Clear();

		private:
			EditorInputManager();

			std::map<KeyCode, KeyboardData> keyboardMap;
			std::map<MouseCode, MouseData> mouseMap;

			static std::unordered_map<int, KeyCode> rawKeyEnumMap;
		};
	}
}
#endif
