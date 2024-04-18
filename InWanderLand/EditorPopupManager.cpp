#include "EditorPopupManager.h"

namespace application
{
	namespace editor
	{
		void EditorPopupManager::Initialize()
		{

		}

		void EditorPopupManager::Clear()
		{
			currentPopup = "";
			for (int i = 0; i < returnStack.size(); i++)
			{
				returnStack.pop();
				returnDataPtr.pop();
			}
		}

		void EditorPopupManager::SetCurrentPopup(const std::string& popupName)
		{
			currentPopup = popupName;
		}

		std::string EditorPopupManager::GetCurrentPopup()
		{
			return currentPopup;
		}

		std::string EditorPopupManager::GetReturnPopupName()
		{
			if (returnStack.empty())
			{
				return std::string();
			}

			return returnStack.top();
		}

		void EditorPopupManager::Return()
		{
			if (returnStack.empty())
			{
				return;
			}

			imgui::GetMessageBox(returnStack.top()).ShouldOpen = true;
			returnStack.pop();
			returnDataPtr.pop();
		}
	}
}