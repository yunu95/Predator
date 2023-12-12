#include "LoadMapCommand.h"

#include "MapFileManager.h"

namespace Application
{
	namespace Editor
	{
		LoadMapCommand::LoadMapCommand()
			: mapFileManager(MapFileManager::GetInstance())
		{

		}

		void LoadMapCommand::Execute()
		{
			mapFileManager.LoadMapFile("TestMap");
		}
	}
}
