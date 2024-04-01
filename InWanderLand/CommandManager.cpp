#include "InWanderLand.h"
#include "CommandManager.h"

namespace application
{
	namespace editor
	{
		CommandManager::CommandManager()
		{

		}

		CommandManager::~CommandManager()
		{

		}

		void CommandManager::ExecuteCommands()
		{
			if (commandQueue.empty() && undoableCommandQueue.empty())
			{
				return;
			}

			for (int i = 0; i < commandQueue.size(); i++)
			{
				commandQueue.front()->Execute();
				if(!commandQueue.empty())
				{
					commandQueue.pop();
				}
			}

			for (int i = 0; i < undoableCommandQueue.size(); i++)
			{
				undoableCommandQueue.front()->Undo();
				if (!undoableCommandQueue.empty())
				{
					undoableCommandQueue.pop();
				}
			}
		}

		void CommandManager::UndoCommand()
		{
			if (undoQueue.empty())
			{
				return;
			}

			auto ptr = undoQueue.back();
			undoableCommandQueue.emplace(ptr);
			redoStack.emplace_front(ptr);
			undoQueue.pop_back();
		}

		void CommandManager::RedoCommand()
		{
			if (redoStack.empty())
			{
				return;
			}

			auto ptr = redoStack.front();
			commandQueue.emplace(ptr);
			undoQueue.emplace_back(ptr);
			redoStack.pop_front();
		}

		void CommandManager::Clear()
		{
			commandQueue = std::queue<std::shared_ptr<Command>>();
			undoableCommandQueue = std::queue<std::shared_ptr<UndoableCommand>>();
			undoQueue.clear();
			redoStack.clear();
		}

		/// private
		void CommandManager::ResizeBuffer()
		{
			while (undoQueue.size() + redoStack.size() > bufferSize)
			{
				undoQueue.pop_front();
			}
		}
	}
}
