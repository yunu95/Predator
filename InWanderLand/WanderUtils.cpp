#include "WanderUtils.h"
#include "GlobalConstant.h"
#include "LightData.h"
#include "Application.h"
#include "ContentsLayer.h"

float wanderUtils::ResourceRecursiveLoader::coroutineDeltaTimeThreshold = 0.02f;
bool wanderUtils::ResourceRecursiveLoader::isLoadingResources = false;
#ifdef EDITOR
std::unordered_map<string, float> wanderUtils::ResourceRecursiveLoader::timeTookToLoadResources;
#endif
bool wanderUtils::ResourceRecursiveLoader::IsLoadingResources()
{
	return isLoadingResources;
}
// 사실상 얘는 로딩화면을 띄우기 위한 최소한의 리소스를 로드하는 데에만 쓰인다.
void wanderUtils::ResourceRecursiveLoader::Load(string rootFolder, std::set<std::string> validExtensions)
{
	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	/// SCRES 우선 로드
	//resourceManager->LoadFile("FBXMaterial.scres");

	// 나머지 기타등등 파일들 로드하기
	{
		namespace fs = std::filesystem;
		std::set<std::string> soundExtensions{ ".wav", ".mp3", ".wma", ".ogg" };
		fs::path basePath{ rootFolder };
		fs::path realRootPath{ "./" };
		try
		{
			if (fs::exists(basePath) && fs::is_directory(basePath))
			{
				for (const auto& entry : fs::recursive_directory_iterator(basePath))
				{
#ifdef EDITOR
					auto startTime = std::chrono::high_resolution_clock::now();
					static vector<string> damnableTextures;
					if (entry.path().extension().string() == ".png" || entry.path().extension().string() == ".jpg" || entry.path().extension().string() == ".bmp" || entry.path().extension().string() == ".tga")
					{
						damnableTextures.push_back(entry.path().string());
					}
#endif
					if (fs::is_regular_file(entry) && validExtensions.contains(entry.path().extension().string()))
					{
						auto relativePath = fs::relative(entry.path(), realRootPath).string();
						std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
						resourceManager->LoadFile(relativePath.c_str());
					}
					else if (fs::is_regular_file(entry) && soundExtensions.contains(entry.path().extension().string()))
					{
						auto relativePath = fs::relative(entry.path(), realRootPath).string();
						std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
						yunutyEngine::SoundSystem::LoadSound(relativePath);
					}
#ifdef EDITOR
					timeTookToLoadResources[entry.path().extension().string()] += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() / 1000.0f;
#endif
				}
			}
		}
		catch (const fs::filesystem_error& err) {
			std::cerr << "Error: " << err.what() << std::endl;
		}
	}
}
coroutine::Coroutine wanderUtils::ResourceRecursiveLoader::LoadByCoroutine(string rootFolder)
{
	boolGuard guard(isLoadingResources);
	isLoadingResources = true;
	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	auto enterTime = std::chrono::high_resolution_clock::now();
#ifdef EDITOR
	auto startTime = std::chrono::high_resolution_clock::now();
#endif
	/// SCRES 우선 로드

	// 기타등등 파일들 로드하기, 이거 잘못하다가 충돌날 수도 있긴 함.
	std::future<void> futureEtc = std::async(std::launch::async, [&]()
		{
			namespace fs = std::filesystem;
			resourceManager->LoadFile("FBXMaterial.scres");
			std::set<std::string> validExtensions{ ".cso" ,".mp4" ,".vfx" };
			std::set<std::string> textureExtensions{ ".jpg", ".bmp", ".tga", ".dds",".png" };
			std::set<std::string> soundExtensions{ ".wav", ".mp3", ".wma", ".ogg" };
			fs::path basePath{ rootFolder };
			try
			{
				std::vector<std::wstring> texturePaths;
				if (fs::exists(basePath) && fs::is_directory(basePath))
				{
					for (const auto& entry : fs::recursive_directory_iterator(basePath))
					{
#ifdef EDITOR
						auto startTime = std::chrono::high_resolution_clock::now();
#endif
						if (fs::is_regular_file(entry) && validExtensions.contains(entry.path().extension().string()))
						{
							auto relativePath = fs::relative(entry.path(), basePath).string();
							std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
							resourceManager->LoadFile(relativePath.c_str());
						}
						else if (fs::is_regular_file(entry) && soundExtensions.contains(entry.path().extension().string()))
						{
							auto relativePath = fs::relative(entry.path(), basePath).string();
							std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
							yunutyEngine::SoundSystem::LoadSound(relativePath);
						}
						else if (fs::is_regular_file(entry) && textureExtensions.contains(entry.path().extension().string()))
						{
							auto relativePath = fs::relative(entry.path(), basePath).string();
							std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
							resourceManager->LoadFile(relativePath.c_str());
							//texturePaths.push_back(relativePath);
						}
#ifdef EDITOR
						timeTookToLoadResources[entry.path().extension().string()] += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() / 1000.0f;
#endif
					}
				}
				resourceManager->CreateTextures(texturePaths);
			}
			catch (const fs::filesystem_error& err) {
				std::cerr << "Error: " << err.what() << std::endl;
			} });
	while (!futureEtc._Is_ready())
	{
		co_await std::suspend_always{};
	}

	// FBX 로드하기
	{
		auto directorList = application::editor::fileSystem::GetSubdirectories("FBX");
#ifdef EDITOR
		startTime = std::chrono::high_resolution_clock::now();
#endif
		// FBX 로드는 스레드로 처리해도 오류발생의 위험은 크게 없는 것 같다.
		std::future<void> futureFBX = std::async(std::launch::async, [&]()
			{
				for (auto each : directorList)
				{
					resourceManager->LoadFile(("FBX/" + each.string()).c_str());
				}
			});
		while (!futureFBX._Is_ready())
		{
			co_await std::suspend_always{};
		}
		// 혹여나 FBX를 스레드를 통해 로드하는 게 자꾸 문제를 일으킨다면 아래의 코루틴 코드를 쓰자.
		//for (auto each : directorList)
		//{
		//    startTime = std::chrono::high_resolution_clock::now();
		//    resourceManager->LoadFile(("FBX/" + each.string()).c_str());
		//    while (IsTimeToReturn(startTime))
		//    {
		//        co_await std::suspend_always{};
		//    }
		//}
#ifdef EDITOR
		timeTookToLoadResources[".fbx"] += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() / 1000.0f;
#endif
	}

#ifdef EDITOR
	application::Application::GetInstance().CheckContentsLayerInit();

	application::Application::GetInstance().layers[(int)application::Application::LayerList::EditorLayer]->Initialize();

	static_cast<application::editor::EditorLayer*>(application::Application::GetInstance().layers[(int)application::Application::LayerList::EditorLayer])->LateInitialize();
#endif

#ifndef GRAPHICS_TEST
	application::editor::MapFileManager::GetSingletonInstance().LoadMapFile("InWanderLand.pmap");
#endif
	editor::TerrainData* terrainData = nullptr;
	for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::TerrainData>())
	{
		terrainData = each;
	}

	if (terrainData != nullptr)
	{
		std::future<void> navBuildFuture = std::async(std::launch::async, [=]()
			{
				terrainData->ApplyAsPlaytimeObject();
			});
		while (!navBuildFuture._Is_ready())
		{
			co_await std::suspend_always{};
		}
	}
}

std::map<wanderUtils::Stage, std::vector<std::function<void()>>> contentsCallbackMap = std::map<wanderUtils::Stage, std::vector<std::function<void()>>>();
wanderUtils::Stage currentStage = wanderUtils::Stage::One;
wanderUtils::Stage currentLightMap = wanderUtils::Stage::One;

wanderUtils::Stage wanderUtils::GetCurrentStage()
{
	return currentStage;
}

wanderUtils::Stage wanderUtils::GetCurrentLightMap()
{
	return currentLightMap;
}

void wanderUtils::ChangeStageToOne()
{
	ChangeLightMap(Stage::One);
	if (contentsCallbackMap.contains(Stage::One))
	{
		for (auto& each : contentsCallbackMap[Stage::One])
		{
			each();
		}
	}

	currentStage = wanderUtils::Stage::One;
}

void wanderUtils::ChangeStageToTwo()
{
	ChangeLightMap(Stage::Two);
	if (contentsCallbackMap.contains(Stage::Two))
	{
		for (auto& each : contentsCallbackMap[Stage::Two])
		{
			each();
		}
	}

	currentStage = wanderUtils::Stage::Two;
}

void wanderUtils::ChangeLightMap(Stage stage)
{
	switch (stage)
	{
		case wanderUtils::Stage::Two:
		{
			graphics::Camera::GetMainCamera()->GetGI().SetClearColor(yunuGI::Color{ 0,0,0,1 });
			yunutyEngine::graphics::Renderer::SingleInstance().SetLightMap(L"Stage2LightMap");
			if (static_cast<contents::ContentsLayer*>(Application::GetInstance().GetContentsLayer())->IsPlayingContents())
			{
				application::editor::LightData::GetPlaytimeDirectionalLight()->GetTransform()->SetWorldRotation(Quaternion{ Vector3d{90,0,0} });
			}
			else
			{
				auto editorLight = application::editor::LightData::GetEditorDirectionalLight();
				auto inGameRot = Quaternion{ Vector3d{90,0,0} };
				editorLight->pod.rotation.x = inGameRot.x;
				editorLight->pod.rotation.y = inGameRot.y;
				editorLight->pod.rotation.z = inGameRot.z;
				editorLight->pod.rotation.w = inGameRot.w;
				editorLight->ApplyAsPaletteInstance();
			}
			currentLightMap = Stage::Two;
			break;
		}
		default:
		{
			graphics::Camera::GetMainCamera()->GetGI().SetClearColor(yunuGI::Color{ 0.7686,0.8784,0.9451,1 });
			yunutyEngine::graphics::Renderer::SingleInstance().SetLightMap(L"Stage1LightMap");
			Quaternion inGameRot;
			inGameRot.x = 0.860572696;
			inGameRot.y = 0.0646217689;
			inGameRot.z = 0.11142046;
			inGameRot.w = 0.492771924;
			if (static_cast<contents::ContentsLayer*>(Application::GetInstance().GetContentsLayer())->IsPlayingContents())
			{
				application::editor::LightData::GetPlaytimeDirectionalLight()->GetTransform()->SetWorldRotation(inGameRot);
			}
			else
			{
				auto editorLight = application::editor::LightData::GetEditorDirectionalLight();
				editorLight->pod.rotation.x = inGameRot.x;
				editorLight->pod.rotation.y = inGameRot.y;
				editorLight->pod.rotation.z = inGameRot.z;
				editorLight->pod.rotation.w = inGameRot.w;
				editorLight->ApplyAsPaletteInstance();
			}
			currentLightMap = Stage::One;
			break;
		}
	}
}

void wanderUtils::ClearContentsCallbacks()
{
	for (auto& [stage, callbacks] : contentsCallbackMap)
	{
		callbacks.clear();
	}
	contentsCallbackMap.clear();
}

void wanderUtils::PushStageCallback(Stage stage, const std::function<void()>& callback)
{
	contentsCallbackMap[stage].push_back(callback);
}

Vector3d wanderUtils::GetInitSpeedOfFreeFall(float duration, Vector3d startPos, Vector3d destPos)
{
	if (duration == 0)
	{
		return Vector3d();
	}

	float acc = GlobalConstant::GetSingletonInstance().pod.gravitySpeed;

	auto deltaPos = destPos - startPos;
	float init_Y = (deltaPos.y / duration) + (acc * duration / 2);

	return Vector3d(deltaPos.x / duration, init_Y, deltaPos.z / duration);
}

namespace wanderUtils
{
	namespace UnitCoroutine
	{
		ForSecondsFromUnit::ForSecondsFromUnit(std::weak_ptr<Unit> unit, float duration) : duration(duration), unit(unit) {}
		bool ForSecondsFromUnit::Tick()
		{
			if (unit.expired() || elapsed == duration)
				return false;
			elapsed += yunutyEngine::Time::GetDeltaTime() * unit.lock()->localTimeScale;
			elapsed = std::min(elapsed, duration);
			return true;
		}
		float ForSecondsFromUnit::Elapsed()
		{
			return elapsed;
		}
		float ForSecondsFromUnit::ElapsedNormalized()
		{
			return elapsed / duration;
		}

		float ForSecondsFromUnit::GetDeltaTime()
		{
			return yunutyEngine::Time::GetDeltaTime() * unit.lock()->localTimeScale;
		}
	}
}
