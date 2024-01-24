#include "InWanderLand.h"
#include "MapFileManager.h"

#include "Storable.h"
#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "UUIDManager.h"

#include <fstream>

namespace application
{
    namespace editor
    {
        MapFileManager::MapFileManager()
            : Singleton<MapFileManager>(), instanceManager(InstanceManager::GetSingletonInstance()), templateDataManager(TemplateDataManager::GetSingletonInstance())
            , currentMapPath()
        {

        }

        bool MapFileManager::LoadMapFile(const std::string& path)
        {
            std::ifstream loadFile{ path };

            if (loadFile.is_open())
            {
                json mapData;
                loadFile >> mapData;

                // 제대로 된 Map 파일이 아닐 경우 취소
                if (mapData.find("InWanderLand") == mapData.end())
                {
                    loadFile.close();
                    return false;
                }

                // Manager 초기화
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

            if (mapData.is_null())
            {
                // mapdData 가 만들어진 것이 없다면 취소
                return false;
            }

            mapData["InWanderLand"] = "Map";

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

        std::string MapFileManager::GetCurrentMapPath() const
        {
            return currentMapPath;
        }

        void MapFileManager::SetCurrentMapPath(std::string& path)
        {
            currentMapPath = path;
        }

        void MapFileManager::Clear()
        {
            instanceManager.Clear();
            templateDataManager.Clear();
            UUIDManager::GetSingletonInstance().Clear();
        }
    }
}
