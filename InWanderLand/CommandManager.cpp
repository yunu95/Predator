#include "CommandManager.h"

namespace Application
{
	namespace Editor
	{
		std::unique_ptr<CommandManager> CommandManager::instance = nullptr;

		CommandManager& CommandManager::GetInstance()
		{
			if (instance == nullptr)
			{
				instance = std::unique_ptr<CommandManager>(new CommandManager());
			}

			return *instance;
		}

		CommandManager::~CommandManager()
		{

		}

		void CommandManager::ExecuteCommands()
		{
			if (commandQueue.empty() == true)
			{
				return;
			}

			for (int i = 0; i < commandQueue.size(); i++)
			{
				commandQueue.front()->Execute();
				commandQueue.pop();
			}
		}

		void CommandManager::UndoCommand()
		{
			auto& ptr = undoQueue.back();
			commandQueue.emplace(ptr);
			redoStack.emplace(ptr);
			undoQueue.pop_back();
		}

		void CommandManager::RedoCommand()
		{
			auto& ptr = redoStack.top();
			commandQueue.emplace(ptr);
			undoQueue.emplace_back(ptr);
			redoStack.pop();
		}

		/// private
		CommandManager::CommandManager()
		{

		}

		void CommandManager::ResizeBuffer()
		{
			while (undoQueue.size() + redoStack.size() > bufferSize)
			{
				undoQueue.pop_front();
			}
		}
	}
}
