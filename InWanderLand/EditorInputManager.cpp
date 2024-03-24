#include "InWanderLand.h"
#include "EditorInputManager.h"
#include "Application.h"
#include "MouseEvents.h"

#include <Windows.h>

namespace application
{
	namespace editor
	{
		std::unordered_map<int, KeyCode> EditorInputManager::rawKeyEnumMap = std::unordered_map<int, KeyCode>();

		EditorInputManager::EditorInputManager()
		{
			rawKeyEnumMap.insert(std::make_pair(VK_ESCAPE, KeyCode::ESC));
			rawKeyEnumMap.insert(std::make_pair(VK_LEFT, KeyCode::LeftArrow));
			rawKeyEnumMap.insert(std::make_pair(VK_RIGHT, KeyCode::RightArrow));
			rawKeyEnumMap.insert(std::make_pair(VK_UP, KeyCode::UpArrow));
			rawKeyEnumMap.insert(std::make_pair(VK_DOWN, KeyCode::DownArrow));
			rawKeyEnumMap.insert(std::make_pair(VK_SPACE, KeyCode::Space));
			rawKeyEnumMap.insert(std::make_pair(VK_RETURN, KeyCode::Return));

			rawKeyEnumMap.insert(std::make_pair(VK_CONTROL, KeyCode::Control));
			rawKeyEnumMap.insert(std::make_pair(VK_SHIFT, KeyCode::Shift));
			rawKeyEnumMap.insert(std::make_pair(VK_MENU, KeyCode::Alt));
			rawKeyEnumMap.insert(std::make_pair(VK_TAB, KeyCode::Tab));
			rawKeyEnumMap.insert(std::make_pair(VK_BACK, KeyCode::Backspace));
			rawKeyEnumMap.insert(std::make_pair(VK_DELETE, KeyCode::Delete));
			rawKeyEnumMap.insert(std::make_pair(VK_INSERT, KeyCode::Insert));
			rawKeyEnumMap.insert(std::make_pair(VK_HOME, KeyCode::Home));
			rawKeyEnumMap.insert(std::make_pair(VK_END, KeyCode::End));
			rawKeyEnumMap.insert(std::make_pair(VK_PRIOR, KeyCode::PgUp));
			rawKeyEnumMap.insert(std::make_pair(VK_NEXT, KeyCode::PgDn));

			rawKeyEnumMap.insert(std::make_pair('A', KeyCode::A));
			rawKeyEnumMap.insert(std::make_pair('B', KeyCode::B));
			rawKeyEnumMap.insert(std::make_pair('C', KeyCode::C));
			rawKeyEnumMap.insert(std::make_pair('D', KeyCode::D));
			rawKeyEnumMap.insert(std::make_pair('E', KeyCode::E));
			rawKeyEnumMap.insert(std::make_pair('F', KeyCode::F));
			rawKeyEnumMap.insert(std::make_pair('G', KeyCode::G));
			rawKeyEnumMap.insert(std::make_pair('H', KeyCode::H));
			rawKeyEnumMap.insert(std::make_pair('I', KeyCode::I));
			rawKeyEnumMap.insert(std::make_pair('J', KeyCode::J));
			rawKeyEnumMap.insert(std::make_pair('K', KeyCode::K));
			rawKeyEnumMap.insert(std::make_pair('L', KeyCode::L));
			rawKeyEnumMap.insert(std::make_pair('M', KeyCode::M));
			rawKeyEnumMap.insert(std::make_pair('N', KeyCode::N));
			rawKeyEnumMap.insert(std::make_pair('O', KeyCode::O));
			rawKeyEnumMap.insert(std::make_pair('P', KeyCode::P));
			rawKeyEnumMap.insert(std::make_pair('Q', KeyCode::Q));
			rawKeyEnumMap.insert(std::make_pair('R', KeyCode::R));
			rawKeyEnumMap.insert(std::make_pair('S', KeyCode::S));
			rawKeyEnumMap.insert(std::make_pair('T', KeyCode::T));
			rawKeyEnumMap.insert(std::make_pair('U', KeyCode::U));
			rawKeyEnumMap.insert(std::make_pair('V', KeyCode::V));
			rawKeyEnumMap.insert(std::make_pair('W', KeyCode::W));
			rawKeyEnumMap.insert(std::make_pair('X', KeyCode::X));
			rawKeyEnumMap.insert(std::make_pair('Y', KeyCode::Y));
			rawKeyEnumMap.insert(std::make_pair('Z', KeyCode::Z));

			rawKeyEnumMap.insert(std::make_pair('1', KeyCode::NUM_1));
			rawKeyEnumMap.insert(std::make_pair('2', KeyCode::NUM_2));
			rawKeyEnumMap.insert(std::make_pair('3', KeyCode::NUM_3));
			rawKeyEnumMap.insert(std::make_pair('4', KeyCode::NUM_4));
			rawKeyEnumMap.insert(std::make_pair('5', KeyCode::NUM_5));
			rawKeyEnumMap.insert(std::make_pair('6', KeyCode::NUM_6));
			rawKeyEnumMap.insert(std::make_pair('7', KeyCode::NUM_7));
			rawKeyEnumMap.insert(std::make_pair('8', KeyCode::NUM_8));
			rawKeyEnumMap.insert(std::make_pair('9', KeyCode::NUM_9));
			rawKeyEnumMap.insert(std::make_pair('0', KeyCode::NUM_0));

			rawKeyEnumMap.insert(std::make_pair(VK_NUMPAD1, KeyCode::NUMPAD_1));
			rawKeyEnumMap.insert(std::make_pair(VK_NUMPAD2, KeyCode::NUMPAD_2));
			rawKeyEnumMap.insert(std::make_pair(VK_NUMPAD3, KeyCode::NUMPAD_3));
			rawKeyEnumMap.insert(std::make_pair(VK_NUMPAD4, KeyCode::NUMPAD_4));
			rawKeyEnumMap.insert(std::make_pair(VK_NUMPAD5, KeyCode::NUMPAD_5));
			rawKeyEnumMap.insert(std::make_pair(VK_NUMPAD6, KeyCode::NUMPAD_6));
			rawKeyEnumMap.insert(std::make_pair(VK_NUMPAD7, KeyCode::NUMPAD_7));
			rawKeyEnumMap.insert(std::make_pair(VK_NUMPAD8, KeyCode::NUMPAD_8));
			rawKeyEnumMap.insert(std::make_pair(VK_NUMPAD9, KeyCode::NUMPAD_9));
			rawKeyEnumMap.insert(std::make_pair(VK_NUMPAD0, KeyCode::NUMPAD_0));

			rawKeyEnumMap.insert(std::make_pair(VK_OEM_PERIOD, KeyCode::Perido));
			rawKeyEnumMap.insert(std::make_pair(VK_OEM_PLUS, KeyCode::Plus));
			rawKeyEnumMap.insert(std::make_pair(VK_OEM_COMMA, KeyCode::Comma));
			rawKeyEnumMap.insert(std::make_pair(VK_OEM_MINUS, KeyCode::Minus));
			rawKeyEnumMap.insert(std::make_pair(VK_OEM_2, KeyCode::Slash));
			rawKeyEnumMap.insert(std::make_pair(VK_OEM_5, KeyCode::BackSlash));

			rawKeyEnumMap.insert(std::make_pair(VK_F1, KeyCode::F1));
			rawKeyEnumMap.insert(std::make_pair(VK_F2, KeyCode::F2));
			rawKeyEnumMap.insert(std::make_pair(VK_F3, KeyCode::F3));
			rawKeyEnumMap.insert(std::make_pair(VK_F4, KeyCode::F4));
			rawKeyEnumMap.insert(std::make_pair(VK_F5, KeyCode::F5));
			rawKeyEnumMap.insert(std::make_pair(VK_F6, KeyCode::F6));
			rawKeyEnumMap.insert(std::make_pair(VK_F7, KeyCode::F7));
			rawKeyEnumMap.insert(std::make_pair(VK_F8, KeyCode::F8));
			rawKeyEnumMap.insert(std::make_pair(VK_F9, KeyCode::F9));
			rawKeyEnumMap.insert(std::make_pair(VK_F10, KeyCode::F10));
			rawKeyEnumMap.insert(std::make_pair(VK_F11, KeyCode::F11));
			rawKeyEnumMap.insert(std::make_pair(VK_F12, KeyCode::F12));
		}

		KeyCode EditorInputManager::GetKeyCode(const int& key)
		{
			if (rawKeyEnumMap.find(key) == rawKeyEnumMap.end())
			{
				return KeyCode::NoKey;
			}

			return rawKeyEnumMap[key];
		}

		void EditorInputManager::Update()
		{
			for (auto& each : keyboardMap)
			{
				if (each.second.now == KeyState::Pressed)
				{
					UpdateKeyboardState(each.second.keyCode, KeyState::Down);
				}
			}

			// Mouse 의 경우, 윈도우에서 메시지가 지속적으로 발생하지 않아 여기서 이벤트를 발생함
			// Map 자체가 가볍기 때문에 Update 마다 조사하여 호출함
			for (auto& each : mouseMap)
			{
				if (each.second.now == KeyState::Pressed)
				{
					UpdateMouseButtonState(each.second.mouseCode, KeyState::Down);
				}
				else if(each.second.now == KeyState::Down)
				{
					Application::DispatchEvent<MouseButtonDownEvent>(each.first);
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

