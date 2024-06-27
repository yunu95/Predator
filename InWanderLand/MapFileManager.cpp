#include "InWanderLand.h"
#include "MapFileManager.h"
#include "Palette.h"
#include "Application.h"
#include "EditorEvents.h"
#include "EditorCommonEvents.h"
#include "WavePalette.h"
#include "OrnamentPalette.h"

#include "GlobalConstant.h"
#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "Ornament_TemplateData.h"
#include "OrnamentData.h"
#include "UUIDManager.h"
#include "PaletteManager.h"
#include "PaletteBrushManager.h"
#include "EditorResourceManager.h"
#include "EditorCameraManager.h"
#include "ShortcutSystem.h"
#include "ScriptSystem.h"
#include "Script.h"
#include "Module_ScriptEditor.h"
#include "PlayableComponent.h"
#include "EditorPopupManager.h"
#include "EditableDataList.h"
#include "SkillManager.h"

#include <fstream>

namespace application
{
	namespace editor
	{
		MapFileManager::MapFileManager()
			: Singleton<MapFileManager>(), globalConstant(GlobalConstant::GetSingletonInstance()),
			instanceManager(InstanceManager::GetSingletonInstance()),
			templateDataManager(TemplateDataManager::GetSingletonInstance()),
			commandManager(CommandManager::GetSingletonInstance()),
			skillManager(SkillManager::GetSingletonInstance()),
			currentMapPath()
		{

		}

		bool MapFileManager::LoadStaticOrnaments(const std::string& path)
		{
			auto& guidMap = InstanceManager::GetSingletonInstance().GetPunrealGUIDMap();
			for (auto& each : InstanceManager::GetSingletonInstance().GetList<OrnamentData>())
			{
				guidMap[String_To_UUID(each->pod.punrealGUID)] = each;
			}

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
				std::vector<double> rotation(4);
				std::vector<float> location(3);
				UUID loadedGUID;
				auto loadedStage = path.find("1Stage") != std::string::npos ? 1 : 2;

				for (int i = 0; i < objSize; i++)
				{
					fbxName = mapData[i]["ResourceName"];
					scale[0] = mapData[i]["Scale"][0];
					scale[1] = mapData[i]["Scale"][1];
					scale[2] = mapData[i]["Scale"][2];
					rotation[0] = mapData[i]["Rotation"][0];
					rotation[1] = mapData[i]["Rotation"][1];
					rotation[2] = mapData[i]["Rotation"][2];
					rotation[3] = mapData[i]["Rotation"][3];
					location[0] = (float)(mapData[i]["Location"][0]) / 100.f;
					location[1] = (float)(mapData[i]["Location"][1]) / 100.f;
					location[2] = (float)(mapData[i]["Location"][2]) / 100.f;

					std::string unworkedGUID = mapData[i]["GUID"];
					unworkedGUID.insert(8, "-");
					unworkedGUID.insert(13, "-");
					unworkedGUID.insert(18, "-");
					unworkedGUID.insert(23, "-");
					loadedGUID = String_To_UUID(unworkedGUID);
					int lightMapIndex = -1;
					if (mapData[i].contains("LightMapIndex"))
						lightMapIndex = (int)(mapData[i]["LightMapIndex"]);
					float LightMapScaleOffset[4] = { 0, 0, 0, 0 };
					if (mapData[i].contains("LightMapScaleOffset"))
					{
						LightMapScaleOffset[0] = (float)(mapData[i]["LightMapScaleOffset"][0]);
						LightMapScaleOffset[1] = (float)(mapData[i]["LightMapScaleOffset"][1]);
						LightMapScaleOffset[2] = (float)(mapData[i]["LightMapScaleOffset"][2]);
						LightMapScaleOffset[3] = (float)(mapData[i]["LightMapScaleOffset"][3]);
					}

					if (guidMap.contains(loadedGUID))
					{
						guidMap[loadedGUID]->pod.scale.x = scale[0];
						guidMap[loadedGUID]->pod.scale.y = scale[2];
						guidMap[loadedGUID]->pod.scale.z = scale[1];
						guidMap[loadedGUID]->pod.rotation.x = -rotation[1];
						guidMap[loadedGUID]->pod.rotation.y = rotation[3];
						guidMap[loadedGUID]->pod.rotation.z = rotation[2];
						guidMap[loadedGUID]->pod.rotation.w = rotation[0];
						guidMap[loadedGUID]->pod.position.x = -location[0];
						guidMap[loadedGUID]->pod.position.y = location[2];
						guidMap[loadedGUID]->pod.position.z = location[1];
						guidMap[loadedGUID]->pod.LightMapIndex = lightMapIndex;
						guidMap[loadedGUID]->pod.LightMapScaleOffset[0] = LightMapScaleOffset[0];
						guidMap[loadedGUID]->pod.LightMapScaleOffset[1] = LightMapScaleOffset[1];
						guidMap[loadedGUID]->pod.LightMapScaleOffset[2] = LightMapScaleOffset[2];
						guidMap[loadedGUID]->pod.LightMapScaleOffset[3] = LightMapScaleOffset[3];
						guidMap[loadedGUID]->pod.punrealGUID = UUID_To_String(loadedGUID);
						guidMap[loadedGUID]->pod.stage = loadedStage;
						guidMap[loadedGUID]->ApplyAsPaletteInstance();



						for (auto child : guidMap[loadedGUID]->GetPaletteInstance()->GetGameObject()->GetChildren())
						{
							if (child->getName() == guidMap[loadedGUID]->pod.templateData->pod.staticFBXName)
							{
								for (auto each : child->GetChildrenRecursively())
								{
									auto staticMeshRenderer = each->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
									if (staticMeshRenderer == nullptr)
										continue;

									if (guidMap[loadedGUID]->pod.LightMapIndex != -1)
									{
										staticMeshRenderer->GetGI().SetLightMapUVIndex(guidMap[loadedGUID]->pod.LightMapIndex);
										staticMeshRenderer->GetGI().SetLightMapUVOffset(guidMap[loadedGUID]->pod.LightMapScaleOffset[2], guidMap[loadedGUID]->pod.LightMapScaleOffset[3]);
										staticMeshRenderer->GetGI().SetLightMapUVScaling(guidMap[loadedGUID]->pod.LightMapScaleOffset[0], guidMap[loadedGUID]->pod.LightMapScaleOffset[1]);
									}
								}
							}
						}

						guidMap.erase(loadedGUID);
						continue;
					}

					auto odt = instanceManager.CreateInstance<OrnamentData>(fbxName);
					if (odt == nullptr)
					{
						continue;
					}

					/// 좌표계 고려 변환
					odt->pod.scale.x = scale[0];
					odt->pod.scale.y = scale[2];
					odt->pod.scale.z = scale[1];
					odt->pod.rotation.x = -rotation[1];
					odt->pod.rotation.y = rotation[3];
					odt->pod.rotation.z = rotation[2];
					odt->pod.rotation.w = rotation[0];
					odt->pod.position.x = -location[0];
					odt->pod.position.y = location[2];
					odt->pod.position.z = location[1];
					odt->pod.LightMapIndex = lightMapIndex;
					odt->pod.LightMapScaleOffset[0] = LightMapScaleOffset[0];
					odt->pod.LightMapScaleOffset[1] = LightMapScaleOffset[1];
					odt->pod.LightMapScaleOffset[2] = LightMapScaleOffset[2];
					odt->pod.LightMapScaleOffset[3] = LightMapScaleOffset[3];
					odt->pod.punrealGUID = UUID_To_String(loadedGUID);

					odt->pod.stage = loadedStage;

					auto pi = odt->ApplyAsPaletteInstance();
				}

				for (auto& [uuid, each] : guidMap)
				{
					if (!each->pod.madeEditor && each->pod.stage == loadedStage)
					{
						palette::OrnamentPalette::SingleInstance().Delete(each);
					}
				}

				guidMap.clear();
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
					ornamentData["Location"].push_back(-ptr->pod.position.x * 100);
					ornamentData["Location"].push_back(ptr->pod.position.z * 100);
					ornamentData["Location"].push_back(ptr->pod.position.y * 100);
					ornamentData["Rotation"].push_back(ptr->pod.rotation.w);
					ornamentData["Rotation"].push_back(-ptr->pod.rotation.x);
					ornamentData["Rotation"].push_back(ptr->pod.rotation.z);
					ornamentData["Rotation"].push_back(ptr->pod.rotation.y);
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

				auto& scriptSystem = ScriptSystem::Instance();

				if (!globalConstant.PreDecoding(mapData) || !scriptSystem.PreDecoding(mapData) || !instanceManager.PreDecoding(mapData) || !templateDataManager.PreDecoding(mapData) || !skillManager.PreDecoding(mapData))
				{
					loadFile.close();
					return false;
				}

				if (!scriptSystem.PostDecoding(mapData) || !instanceManager.PostDecoding(mapData) || !templateDataManager.PostDecoding(mapData) || !skillManager.PostDecoding(mapData))
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
				Application::GetInstance().OnDataLoad();
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

			auto& scriptSystem = ScriptSystem::Instance();

			// 저장을 더 진행하기 전에 각 데이터들의 내부 데이터를 pod 데이터로 이전하기 위해 실행하는 부분
			if (!instanceManager.PreSave() || !templateDataManager.PreSave())
			{
				return false;
			}

			// Pre
			if (!globalConstant.PreEncoding(mapData) || !scriptSystem.PreEncoding(mapData) || !instanceManager.PreEncoding(mapData) || !templateDataManager.PreEncoding(mapData) || !skillManager.PreEncoding(mapData))
			{
				return false;
			}

			// Post
			if (!scriptSystem.PostEncoding(mapData) || !instanceManager.PostEncoding(mapData) || !templateDataManager.PostEncoding(mapData) || !skillManager.PostEncoding(mapData))
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
			bool returnVal = false;
			if (currentMapPath.empty())
			{
				returnVal = LoadMapFile("InWanderLand.pmap");
				if (returnVal == false)
				{
					Application::GetInstance().OnDataLoad();
				}
				currentMapPath.clear();
			}
			return returnVal;
		}

		void MapFileManager::Clear()
		{
#ifdef EDITOR
			palette::PaletteBrushManager::GetSingletonInstance().Clear();
			palette::PaletteManager::GetSingletonInstance().Clear();
			CameraManager::GetSingletonInstance().Clear();
			Module_ScriptEditor::GetSingletonInstance().Clear();
			EditorPopupManager::GetSingletonInstance().Clear();
			commandManager.Clear();
#endif
			ShortcutSystem::Instance().Clear();
			PlayableComponent::PreMapLoadAll();
			instanceManager.Clear();
			templateDataManager.Clear();
			skillManager.Clear();
			UUIDManager::GetSingletonInstance().Clear();
		}
	}
}
