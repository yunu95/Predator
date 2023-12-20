#include "SaveMapCommand.h"

#include "MapFileManager.h"

namespace application
{
	namespace editor
	{
		SaveMapCommand::SaveMapCommand()
			: mapFileManager(MapFileManager::GetInstance())
		{
			
		}

		void SaveMapCommand::Execute()
		{
			mapFileManager.SaveMapFile("TestMap.pmap");
		}
	}
}
