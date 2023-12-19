#include "MapFileManager.h"

#include "Storable.h"
#include "InstanceManager.h"
#include "TemplateDataManager.h"

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
                auto& templateManager = TemplateDataManager::GetInstance();

                json mapData;
                loadFile >> mapData;

                // Manager √ ±‚»≠
                instanceManager.Clear();
                templateManager.Clear();

                if (!instanceManager.PreDecoding(mapData) || !templateManager.PreDecoding(mapData))
                {
                    loadFile.close();
                    return false;
                }

                if (!instanceManager.PostDecoding(mapData) || !templateManager.PostDecoding(mapData))
                {
                    loadFile.close();
                    return false;
                }
                else
                {
                    loadFile.close();
                    return true;
                }
            }
            else
            {
                return false;
            }
        }

        bool MapFileManager::SaveMapFile(const std::string& path)
        {
            auto& templateManager = TemplateDataManager::GetInstance();

            json mapData;
            
            // Pre
            if (!instanceManager.PreEncoding(mapData) || !templateManager.PreEncoding(mapData))
            {
                return false;
            }

            // Post
            if (!instanceManager.PostEncoding(mapData) || !templateManager.PostEncoding(mapData))
            {
                return false;
            }

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
