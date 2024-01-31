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
			MapFileManager::GetSingletonInstance().LoadMapFile("TestMap.pmap");
            //MapData::LoadMapData("TestMap.pmap");
		}
	}
}
