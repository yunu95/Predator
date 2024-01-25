#include "InWanderLand.h"
#include "LoadMapCommand.h"
#include "MapData.h"
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
            MapData::LoadMapData("TestMap.pmap");
		}
	}
}
