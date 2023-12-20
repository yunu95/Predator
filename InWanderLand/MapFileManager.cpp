#include "MapFileManager.h"

#include "Storable.h"
#include "InstanceManager.h"
#include "TemplateDataManager.h"

#include <fstream>

namespace application
{
    namespace editor
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

                // Manager √ ±‚»≠
                Clear();

                if (!instanceManager.PreDecoding(mapData) || !templateDataManager.PreDecoding(mapData))
                {
                    loadFile.close();
                    return false;
                }

                if (!instanceManager.PostDecoding(mapData) || !templateDataManager.PostDecoding(mapData))
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
            json mapData;
            
            // Pre
            if (!instanceManager.PreEncoding(mapData) || !templateDataManager.PreEncoding(mapData))
            {
                return false;
            }

            // Post
            if (!instanceManager.PostEncoding(mapData) || !templateDataManager.PostEncoding(mapData))
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

        void MapFileManager::Clear()
        {
            instanceManager.Clear();
            templateDataManager.Clear();
        }

        /// private
        MapFileManager::MapFileManager()
            : instanceManager(InstanceManager::GetInstance()), templateDataManager(TemplateDataManager::GetInstance())
            , currentMap()
        {

        }

    }
}
