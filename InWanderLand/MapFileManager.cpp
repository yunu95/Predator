#include "MapFileManager.h"

#include "Storable.h"
#include "InstanceManager.h"

#include <fstream>

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
            std::ifstream loadFile{ path };

            if (loadFile.is_open())
            {
                json mapData;
                loadFile >> mapData;

                if (instanceManager.Decoding(mapData))
                {
                    loadFile.close();
                    return true;
                }
                else
                {
                    loadFile.close();
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        bool MapFileManager::SaveMapFile(const std::string& path)
        {
            json mapData = instanceManager.Encoding();
            
            std::ofstream saveFile{ path };

            if (saveFile.is_open())
            {
                saveFile << mapData.dump(4);
                saveFile.close();
                return true;
            }
            else
            { 
                return false;
            }
        }

        /// private
        MapFileManager::MapFileManager()
            : instanceManager(InstanceManager::GetInstance()), currentMap()
        {

        }

    }
}
