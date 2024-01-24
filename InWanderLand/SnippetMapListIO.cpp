#if defined(GEN_TESTS) && defined(EDITOR)
#include "InWanderLand.h"
#include "CppUnitTest.h"
#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "MapFileManager.h"
#include "TemplateDataManager.h"
#include "InstanceManager.h"
#include "Storable.h"
#include "Terrain_TemplateData.h"

#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void SnippetMapListIOInitializer()
{
    using namespace application::editor;
    auto& tdm = application::editor::TemplateDataManager::GetSingletonInstance();
    auto& mfm = application::editor::MapFileManager::GetSingletonInstance();
    auto& im = application::editor::InstanceManager::GetSingletonInstance();

    // 공백 파일을 생성하기 위한 세이브
    application::editor::Terrain_TemplateData* terrainTemplateData;
    terrainTemplateData = tdm.CreateTemplateData<Terrain_TemplateData>("TerrainData");
    terrainTemplateData->pod.coordinates = { {1,4},{2,3},{6,2},{4,3} };
    mfm.SaveMapFile("SnippetMapListIO.pmap");
    mfm.Clear();
    mfm.LoadMapFile("SnippetMapListIO.pmap");
    terrainTemplateData = tdm.GetTemplateData<Terrain_TemplateData>("TerrainData");
}

namespace snippets
{
    TEST_CLASS(SnippetMapListIOClass)
    {
    public:
        TEST_METHOD(SnippetMapListIO)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::editor::EditorLayer::AssignTestInitializer(SnippetMapListIOInitializer);
            client.Initialize();
        }
    };
}
#endif