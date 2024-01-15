#include "InWanderLand.h"
#include "LoadMapCommand.h"

#include "MapFileManager.h"

namespace application
{
	namespace editor
	{
		LoadMapCommand::LoadMapCommand()
			: mapFileManager(MapFileManager::GetSingletonInstance())
		{

		}

		void LoadMapCommand::Execute()
		{
			mapFileManager.LoadMapFile("TestMap.pmap");
		}
	}
}
