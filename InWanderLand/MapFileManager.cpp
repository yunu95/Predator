#include "InWanderLand.h"
#include "MapFileManager.h"
#include "Palette.h"
#include "Application.h"
#include "EditorEvents.h"
#include "EditorCommonEvents.h"
#include "WavePalette.h"

#include "GlobalConstant.h"
#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "Ornament_TemplateData.h"
#include "OrnamentData.h"
#include "UUIDManager.h"
#include "PaletteManager.h"
#include "PaletteBrushManager.h"
#include "EditorResourceManager.h"

#include <fstream>

namespace application
{
    namespace editor
    {
        MapFileManager::MapFileManager()
            : Singleton<MapFileManager>(), globalConstant(GlobalConstant::GetSingletonInstance()), 
            instanceManager(InstanceManager::GetSingletonInstance()), 
            templateDataManager(TemplateDataManager::GetSingletonInstance()),
            currentMapPath()
        {

        }

        bool MapFileManager::LoadStaticOrnaments(const std::string& path)
        {
            std::ifstream loadFile{ path };

            if (loadFile.is_open())
            {
                json mapData;
                loadFile >> mapData;

                auto scene = yunutyEngine::Scene::getCurrentScene();
                const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

                auto objSize = mapData.size();

                std::string fbxName;
                std::vector<float> scale(3);
                std::vector<double> rotation(3);
                std::vector<float> location(3);

                for (int i = 0; i < objSize; i++)
                {
                    fbxName = mapData[i]["ResourceName"];
                    scale[0] = mapData[i]["Scale"][0];
                    scale[1] = mapData[i]["Scale"][1];
                    scale[2] = mapData[i]["Scale"][2];
                    rotation[0] = mapData[i]["Rotation"][0];
                    rotation[1] = mapData[i]["Rotation"][1];
                    rotation[2] = mapData[i]["Rotation"][2];
                    location[0] = (float)(mapData[i]["Location"][0]) / 100.f;
                    location[1] = (float)(mapData[i]["Location"][1]) / 100.f;
                    location[2] = (float)(mapData[i]["Location"][2]) / 100.f;

                    auto odt = instanceManager.CreateInstance<OrnamentData>(fbxName);

                    /// 좌표계 고려 변환
                    odt->pod.scale.x = scale[0];
                    odt->pod.scale.y = scale[2];
                    odt->pod.scale.z = scale[1];
                    auto quat = Quaternion(Vector3d(rotation[0], rotation[2], -rotation[1]));
                    odt->pod.rotation.x = quat.x;
                    odt->pod.rotation.y = quat.y;
                    odt->pod.rotation.z = quat.z;
                    odt->pod.rotation.w = quat.w;
                    odt->pod.position.x = -location[0];
                    odt->pod.position.y = location[2];
                    odt->pod.position.z = location[1];

                    auto pi = odt->ApplyAsPaletteInstance();
                }

#ifdef EDITOR
                Application::DispatchEvent<LoadEvent>();
#endif
                return true;
            }
            else
            {
                return false;
            }
        }

        bool MapFileManager::SaveStaticOrnaments(const std::string& path)
        {
            json punrealData;
            json ornamentData;

            for (auto& each : instanceManager.list)
            {
                auto ptr = dynamic_cast<OrnamentData*>(each.second.get());
                if (ptr)
                {
                    ornamentData["ResourceName"] = ptr->pod.templateData->pod.staticFBXName;
                    ornamentData["Location"].push_back(ptr->pod.position.x * 100);
                    ornamentData["Location"].push_back(-ptr->pod.position.z * 100);
                    ornamentData["Location"].push_back(ptr->pod.position.y * 100);
                    Quaternion quat;
                    quat.w = ptr->pod.rotation.w;
                    quat.x = ptr->pod.rotation.x;
                    quat.y = ptr->pod.rotation.y;
                    quat.z = ptr->pod.rotation.z;
                    ornamentData["Rotation"].push_back(-quat.Euler().x);
                    ornamentData["Rotation"].push_back(quat.Euler().z);
                    ornamentData["Rotation"].push_back(quat.Euler().y);
                    ornamentData["Scale"].push_back(ptr->pod.scale.x);
                    ornamentData["Scale"].push_back(ptr->pod.scale.z);
                    ornamentData["Scale"].push_back(ptr->pod.scale.y);

                    punrealData.push_back(ornamentData);
                    ornamentData.clear();
                }
            }

            if (punrealData.is_null())
            {
                return false;
            }

            std::ofstream saveFile{ path };

            if (saveFile.is_open())
            {
                saveFile << punrealData.dump(4);
                saveFile.close();

                return true;
            }
            else
            {
                return false;
            }
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

                if (!globalConstant.PreDecoding(mapData) || !instanceManager.PreDecoding(mapData) || !templateDataManager.PreDecoding(mapData))
                {
                    loadFile.close();
                    return false;
                }

                if (!instanceManager.PostDecoding(mapData) || !templateDataManager.PostDecoding(mapData))
                {
                    loadFile.close();
                    return false;
                }

                if (!instanceManager.PostLoad())
                {
                    return false;
                }

                loadFile.close();

#ifdef EDITOR
                Application::DispatchEvent<LoadEvent>();

                // 기본 Ornament 추가
                Application::GetInstance().ReadyOrnament();
                palette::PaletteBrushManager::GetSingletonInstance().MakeBrush();
#endif
                currentMapPath = path;
                return true;
            }
            else
            {
                return false;
            }
        }

        bool MapFileManager::SaveMapFile(const std::string& path)
        {
            json mapData;
            // 저장을 더 진행하기 전에 각 데이터들의 내부 데이터를 pod 데이터로 이전하기 위해 실행하는 부분
            if (!instanceManager.PreSave() || !templateDataManager.PreSave())
            {
                return false;
            }
            // Pre
            if (!globalConstant.PreEncoding(mapData) || !instanceManager.PreEncoding(mapData) || !templateDataManager.PreEncoding(mapData))
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

#ifdef EDITOR
                Application::DispatchEvent<SaveEvent>();
#endif

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

        void MapFileManager::SetCurrentMapPath(const std::string& path)
        {
            currentMapPath = path;
        }

        bool MapFileManager::LoadDefaultMap()
        {
            bool returnVal = LoadMapFile("Default.pmap");
            Application::GetInstance().ReadyOrnament();
            palette::PaletteBrushManager::GetSingletonInstance().MakeBrush();
            currentMapPath.clear();
            
            return returnVal;
        }

        void MapFileManager::Clear()
        {
#ifdef EDITOR
            palette::PaletteBrushManager::GetSingletonInstance().Clear();
            palette::PaletteManager::GetSingletonInstance().Clear();
#endif
            instanceManager.Clear();
            templateDataManager.Clear();
            UUIDManager::GetSingletonInstance().Clear();
        }
    }
}
