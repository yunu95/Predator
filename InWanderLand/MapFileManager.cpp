#include "MapFileManager.h"

namespace Application
{
    namespace Editor
    {
        std::unique_ptr<MapFileManager> MapFileManager::instance = nullptr;

        MapFileManager& MapFileManager::GetInstance()
        {
			if (instance == nullptr)
			{
				instance = std::unique_ptr<MapFileManager>(new MapFileManager());
			}

            return *instance;
        }

        MapFileManager::~MapFileManager()
        {

        }

        bool MapFileManager::LoadMapFile(const std::string& path)
        {
            return false;
        }

        bool MapFileManager::SaveMapFile(const std::string& path)
        {
            return false;
        }

        /// private
        MapFileManager::MapFileManager()
            : currentMap()
        {

        }

    }
}
