/// 2023. 10. 19 김상준
/// 커맨드들을 일괄적으로 처리하기 위한 관리자 클래스

#pragma once

#include "Singleton.h"
#include "Command.h"

#include <memory>
#include <queue>
#include <deque>
#include <concepts>

namespace application
{
	namespace editor
	{
		class CommandManager
			: public Singleton<CommandManager>
		{
			friend class Singleton<CommandManager>;
			friend class EditorLayer;

			/// 원래는 해당 방식으로 처리하면 안되지만,
			/// 임시로 Delete 에 대해서 직접 접근을 허용하여 처리하도록 함
			friend class DeleteInstanceCommand;

		public:	
			virtual ~CommandManager();
			
			// commandQueue 에 커맨드를 추가
			template<class T, class... Types> requires (std::is_base_of_v<Command, T> && !std::is_base_of_v<UndoableCommand, T>)
			void AddQueue(const std::shared_ptr<T>& command)
			{
				commandQueue.emplace(command);
			}

			// Undo / Redo 가능한 유저 명령은 따로 관리하기 위함 함수
			/// AddQueue 함수 내부에서 dynamic_cast 를 통해서 구현할 수도 있으나,
			/// UndoableCommand 를 위해서 모든 Command 가 dynamic_cast 를 하는 구조가 비효율적이라 판단,
			/// 처음 개발 시점에서 UndoableCommand 는 따로 관리할 수 있도록 처리함
			template<class T> requires std::derived_from<T, UndoableCommand>
			void AddQueue(const std::shared_ptr<T>& command)
			{
				commandQueue.emplace(command);
				undoQueue.emplace_back(command);
				redoStack.clear();

				ResizeBuffer();
			}

			void ExecuteCommands();											// commandQueue 의 내용을 모두 실행함

			void UndoCommand();												// 마지막으로 실행했던 커맨드를 취소하고 되돌림
			void RedoCommand();												// 실행 취소했던 커맨드를 다시 실행함

			void Clear();

		private:
			CommandManager();

			void ResizeBuffer();											// undoQueue 와 redoStack 사이즈를 합친 bufferSize 가 넘칠 때 동작하는 함수

			int bufferSize = 50;
			std::queue<std::shared_ptr<Command>> commandQueue;
			std::queue<std::shared_ptr<UndoableCommand>> undoableCommandQueue;
			std::deque<std::shared_ptr<UndoableCommand>> undoQueue;
			std::deque<std::shared_ptr<UndoableCommand>> redoStack;
		};
	}
}
