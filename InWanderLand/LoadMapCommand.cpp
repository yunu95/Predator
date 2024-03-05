#include "InWanderLand.h"

#include "LoadMapCommand.h"
#include "MapFileManager.h"
#include "FileSystem.h"

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
			std::filesystem::path filepath = fileSystem::LoadFileDialog("Map File (*.pmap)\0*.pmap\0");

			if (filepath.empty())
				return;

			if (!filepath.has_extension())
				filepath += ".pmap";

			MapFileManager::GetSingletonInstance().LoadMapFile(filepath.string());
		}
	}
}
