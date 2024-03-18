#include "InWanderLand.h"

#include "SaveMapCommand.h"
#include "MapFileManager.h"
#include "FileSystem.h"

namespace application
{
    namespace editor
    {
        SaveMapCommand::SaveMapCommand(bool saveAs)
            : saveAs(saveAs), mapFileManager(MapFileManager::GetSingletonInstance())
        {

        }

        void SaveMapCommand::Execute()
        {
            if (saveAs)
            {
                std::filesystem::path filepath = fileSystem::SaveFileDialog("Map File (*.pmap)\0*.pmap\0");

                if (filepath.empty())
                    return;

                if (!filepath.has_extension())
                    filepath += ".pmap";

                mapFileManager.SetCurrentMapPath(filepath.string());
                mapFileManager.SaveMapFile(filepath.string());
            }
            else
            {
                if (mapFileManager.GetCurrentMapPath().empty())
                {
                    return;
                }
                mapFileManager.SaveMapFile(mapFileManager.GetCurrentMapPath());
            }
        }
    }
}
