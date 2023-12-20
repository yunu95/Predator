#include "LoadMapCommand.h"

#include "MapFileManager.h"

namespace application
{
	namespace editor
	{
		LoadMapCommand::LoadMapCommand()
			: mapFileManager(MapFileManager::GetInstance())
		{

		}

		void LoadMapCommand::Execute()
		{
			mapFileManager.LoadMapFile("TestMap.pmap");
		}
	}
}
