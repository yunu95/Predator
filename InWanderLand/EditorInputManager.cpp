#include "InWanderLand.h"
#include "EditorInputManager.h"

namespace application
{
	namespace editor
	{
		void EditorInputManager::Update()
		{
			// Pressed 상태의 키들을 모두 Down 상태로 변경
			for (auto& each : keyboardMap)
			{
				if (each.second.now == KeyState::Pressed)
				{
					UpdateKeyboardState(each.second.keyCode, KeyState::Down);
				}
			}

			for (auto& each : mouseMap)
			{
				if (each.second.now == KeyState::Pressed)
				{
					UpdateMouseButtonState(each.second.mouseCode, KeyState::Down);
				}
			}
		}

		bool EditorInputManager::IsKeyboardPressed(KeyCode keyCode) const
		{
			return (keyboardMap.find(keyCode) != keyboardMap.end()) && (keyboardMap.at(keyCode).now == KeyState::Pressed);
		}

		bool EditorInputManager::IsKeyboardDown(KeyCode keyCode) const
		{
			return (keyboardMap.find(keyCode) != keyboardMap.end()) && (keyboardMap.at(keyCode).now == KeyState::Down);
		}

		bool EditorInputManager::IsKeyboardUp(KeyCode keyCode) const
		{
			return (keyboardMap.find(keyCode) != keyboardMap.end()) && (keyboardMap.at(keyCode).now == KeyState::Up);
		}

		bool EditorInputManager::IsMouseButtonPressed(MouseCode mouseCode) const
		{
			return (mouseMap.find(mouseCode) != mouseMap.end()) && (mouseMap.at(mouseCode).now == KeyState::Pressed);
		}

		bool EditorInputManager::IsMouseButtonDown(MouseCode mouseCode) const
		{
			return (mouseMap.find(mouseCode) != mouseMap.end()) && (mouseMap.at(mouseCode).now == KeyState::Down);
		}

		bool EditorInputManager::IsMouseButtonUp(MouseCode mouseCode) const
		{
			return (mouseMap.find(mouseCode) != mouseMap.end()) && (mouseMap.at(mouseCode).now == KeyState::Up);
		}

		void EditorInputManager::UpdateKeyboardState(KeyCode keycode, KeyState newState)
		{
			auto& keyboardData = keyboardMap[keycode];
			keyboardData.keyCode = keycode;
			keyboardData.old = keyboardData.now;
			keyboardData.now = newState;
		}

		void EditorInputManager::UpdateMouseButtonState(MouseCode mouseCode, KeyState newState)
		{
			auto& mouseData = mouseMap[mouseCode];
			mouseData.mouseCode = mouseCode;
			mouseData.old = mouseData.now;
			mouseData.now = newState;
		}

		void EditorInputManager::Clear()
		{
			for (auto& each : keyboardMap)
			{
				if (each.second.now == KeyState::Up)
				{
					UpdateKeyboardState(each.second.keyCode, KeyState::None);
				}
			}

			for (auto& each : mouseMap)
			{
				if (each.second.now == KeyState::Up)
				{
					UpdateMouseButtonState(each.second.mouseCode, KeyState::None);
				}
			}
		}
	}
}
