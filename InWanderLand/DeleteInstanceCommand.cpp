#include "DeleteInstanceCommand.h"

#include "CommandManager.h"
#include "TransformEditCommand.h"

namespace application
{
	namespace editor
	{
		DeleteInstanceCommand::DeleteInstanceCommand(const std::vector<IEditableData*>& list)
		{
			this->list = list;
		}

		void DeleteInstanceCommand::Execute()
		{
			/// Observer Pattern 으로 변경하여 구현함
			//for (auto& each : CommandManager::GetSingletonInstance().undoQueue)
			//{
			//	auto ptr = dynamic_cast<TransformEditCommand*>(each.get());
			//	if (ptr)
			//	{
			//		for (auto each : list)
			//		{
			//			for (auto& [key, start, end] : ptr->transformList)
			//			{
			//				if (key == each)
			//				{
			//					key = nullptr;
			//				}
			//			}
			//		}
			//	}
			//}

			//for (auto& each : CommandManager::GetSingletonInstance().redoStack)
			//{
			//	auto ptr = dynamic_cast<TransformEditCommand*>(each.get());
			//	if (ptr)
			//	{
			//		for (auto each : list)
			//		{
			//			for (auto& [key, start, end] : ptr->transformList)
			//			{
			//				if (key == each)
			//				{
			//					key = nullptr;
			//				}
			//			}
			//		}
			//	}
			//}
		}
	}
}